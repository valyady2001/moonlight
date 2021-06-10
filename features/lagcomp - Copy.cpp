#include <features.h>


bool HandleBoneSetup( base_entity* target , matrix_t* pBoneToWorldOut , int boneMask , float currentTime )
{
	// wanted to remove as antipaste but tommy wanted to keep this in, give him some feedback, this is the only thing worth looking at in here anyway
	auto hdr = target->get_model_ptr( );
	if ( !hdr )
		return false;

	auto oldBones = target->GetBoneArrayForWrite( );

	matrix_t baseMatrix;
	math::AngleMatrix( target->abs_angles( ) , target->abs_origin( ) , baseMatrix );

	target->effects( ) |= 0x008;

	ik_context* IK_context = target->get_ik_context( );
	if ( IK_context )
	{
		IK_context->clear_targets( );
		IK_context->init( hdr , target->abs_angles( ) , target->origin( ) ,
			interfaces::global_vars->curtime , interfaces::global_vars->framecount , bone_used_by_server );
	}

	vec3_t* pos = ( vec3_t* ) ( interfaces::mem_alloc->alloc( sizeof( vec3_t[ 256 ] ) ) );
	quaternion* q = ( quaternion* ) ( interfaces::mem_alloc->alloc( sizeof( quaternion[ 256 ] ) ) );
	std::memset( pos , 0xFF , sizeof( pos ) );
	std::memset( q , 0xFF , sizeof( q ) );

	/**target->GetBoneArrayForWrite() = *pBoneToWorldOut;*/
	target->GetBoneArrayForWrite( ) = pBoneToWorldOut;

	target->StandardBlendingRules( hdr , pos , q , currentTime , boneMask );

	byte* boneComputed = ( byte* ) ( interfaces::mem_alloc->alloc( sizeof( byte[ 256 ] ) ) );
	std::memset( boneComputed , 0 , sizeof( byte[ 256 ] ) );

	if ( IK_context )
	{
		target->update_ik_locks( interfaces::global_vars->curtime );
		IK_context->update_targets( pos , q , pBoneToWorldOut , &boneComputed[ 0 ] );
		target->calculate_ik_locks( interfaces::global_vars->curtime );
		IK_context->solve_dependencies( pos , q , pBoneToWorldOut , &boneComputed[ 0 ] );
	}

	target->BuildTransformations( hdr , pos , q , baseMatrix , boneMask , &boneComputed[ 0 ] );

	target->effects( ) &= ~0x008;

	//*target->GetBoneArrayForWrite() = *oldBones;
	target->GetBoneArrayForWrite( ) = oldBones;
	return true;
}

bool setup_bones( base_entity* target , int boneMask , float currentTime , matrix_t* pBoneToWorldOut )
{
	alignas( 16 ) matrix_t bone_out[ 128 ];
	const auto ret = HandleBoneSetup( target , bone_out , boneMask , interfaces::global_vars->curtime );
	memcpy( pBoneToWorldOut , bone_out , sizeof( matrix_t[ 128 ] ) );
	return ret;
}


void create_fake_matrix( base_entity* pl , features::lagcomp::records* rec , matrix_t bones[ 128 ] , int delta )
{
	if ( !rec )
		return;

	animlayer backup_layers[ 13 ];
	pl->GetAnimLayers( backup_layers );

	animstate state;
	memcpy( &state , pl->get_animstate( ) , sizeof( animstate ) );

	const auto backup_occlusion_flags = pl->m_iOcclusionFlags( );
	const auto backup_occlusion_framecount = pl->m_iOcclusionFramecount( );

	// skip occlusion checks in c_cs_player::setup_bones.
	if ( pl != g::local )
	{
		pl->m_iOcclusionFlags( ) = 0;
		pl->m_iOcclusionFramecount( ) = 0;
	}

	// clear bone accessor.
	pl->GetReadableBones( ) = pl->GetWriteableBones( ) = 0;

	// invalidate bone cache.
	pl->GetMostRecentModelBoneCounter( ) = 0;
	pl->GetLastBoneSetupTime( ) = -FLT_MAX;

	// stop interpolation.
	pl->effects( ) |= 8;
	pl->get_animstate( )->abs_yaw = pl->get_animstate( )->eye_yaw + delta;
	pl->update_animations( );

	// build bones.
	setup_bones( pl , bone_used_by_anything , interfaces::global_vars->curtime , bones );

	pl->SetAnimLayers( backup_layers );
	memcpy( pl->get_animstate( ) , &state , sizeof( animstate ) );

	// restore original occlusion.
	if ( pl != g::local )
	{
		pl->m_iOcclusionFlags( ) = backup_occlusion_flags;
		pl->m_iOcclusionFramecount( ) = backup_occlusion_framecount;
	}

	// start interpolation again.
	pl->effects( ) &= ~8;
}


namespace features
{
	namespace lagcomp {
		records* get_records( base_entity* e , bool last )
		{
			if ( !e || !e->is_alive( ) )
				return nullptr;
			if ( !last )
			{
				for ( auto it = rec[ e->idx( ) ].begin( ); it != rec[ e->idx( ) ].end( ); it = next( it ) )
				{
					if ( is_valid( ( &*it )->simtime ) && !( &*it )->dormant && ( &*it )->valid )
						return ( &*it );
				}
			}
			else
			{
				for ( auto it = rec[ e->idx( ) ].rbegin( ); it != rec[ e->idx( ) ].rend( ); it = next( it ) )
				{
					if ( is_valid( ( &*it )->simtime ) && !( &*it )->dormant && ( &*it )->valid )
						return ( &*it );
				}
			}
			return nullptr;
		}
		float lerp_time( )
		{
			static c_convar* cl_interp;
			static c_convar* cl_interp_r;
			static c_convar* cl_updaterate;
			if ( !cl_interp )
				cl_interp = interfaces::cvar->find_var( "cl_interp" );
			if ( !cl_interp_r )
				cl_interp_r = interfaces::cvar->find_var( "cl_interp_ratio" );

			if ( !cl_updaterate )
				cl_updaterate = interfaces::cvar->find_var( "cl_updaterate" );

			return max( cl_interp->get_float( ) , cl_interp_r->get_float( ) / cl_updaterate->get_float( ) );
		}

		bool is_valid( float simtime )
		{
			const auto channel_info = interfaces::engine->get_net_channel_info( );

			if ( !channel_info )
				return false;
			static auto sv_maxunlag = interfaces::cvar->find_var( "sv_maxunlag" );

			float max_unlag = 0.2f;

			if ( sv_maxunlag )
				max_unlag = sv_maxunlag->get_float( );

			float correct = channel_info->get_latency( 1 ) + channel_info->get_latency( 0 );
			std::clamp( correct , 0.f , max_unlag );

			return std::fabs( correct - ( antiaim::server_curtime( ) - simtime ) ) <= max_unlag;
		}

		void update_player( base_entity* pl , float& simtime )
		{
			auto state = pl->get_animstate( );
			if ( !state )
				return;

			static bool& invalidate_bone_cache = **reinterpret_cast< bool** >( ( uint32_t ) ( utilities::find_sig( "client.dll" , "C6 05 ? ? ? ? ? 89 47 70" ) + 2 ) );


			const float curtime = interfaces::global_vars->curtime;
			const float frametime = interfaces::global_vars->frametime;
			const float sim = pl->simtime( );
			interfaces::global_vars->curtime = simtime;
			interfaces::global_vars->frametime = interfaces::global_vars->interval_per_tick;

			pl->eflags( ) &= ~0x1000; // EFL_DIRTY_ABSVELOCITY

			if ( state->last_clientside_anim_framecount == interfaces::global_vars->framecount )
				state->last_clientside_anim_framecount -= 1;

			const bool backup_invalidate_bone_cache = invalidate_bone_cache;

			pl->client_animations( ) = true;
			pl->update_animations( );
			pl->client_animations( ) = false;

			pl->invalidate_physics( 0x2a );

			invalidate_bone_cache = backup_invalidate_bone_cache;
			interfaces::global_vars->curtime = curtime;
			interfaces::global_vars->frametime = frametime;

			pl->simtime( ) = sim;
		}

		void __declspec( naked ) feet_wobble_fix( )
		{
			__asm
			{
				push edi
				xchg dword ptr[ esp ] , edi
				push eax
				mov eax , 77
				mov eax , dword ptr[ esp ]
				add esp , 4
				pop edi

				cmp esp , 0
				jne fixentity

				_emit 0x88
				_emit 0xFF

				invlpg dword ptr[ eax ]

				int 2

				fixentity:
				sub esp , 4
					mov dword ptr[ esp ] , ebp

					call cleanup

					pop ebp

					ret

					cleanup :

				ret
			}
		}

		void update_anims( base_entity* e , records* cur )
		{
			records o_rec( e );

			player_info_t info;
			interfaces::engine->get_player_info( e->idx( ) , &info );
			if ( !info.fakeplayer )
			{
				vec3_t vecVelocity = e->velocity( );
				if ( rec[ e->idx( ) ].size( ) >= 2 )
				{
					auto prev = &rec[ e->idx( ) ][ 1 ];
					if ( prev && !prev->dormant )
					{
						e->SetAnimLayers( cur->layers );
						e->set_abs_origin( cur->orig );
						e->velocity( ) = prev->vel;

						cur->vel = vecVelocity;

						for ( int i = 0; i < cur->choked; ++i )
						{
							float simtime = prev->simtime + ticks_to_time( i + 1 );
							const auto lerp = 1.f - ( cur->simtime - simtime ) / ( cur->simtime - prev->simtime );
							e->duck_amount( ) = math::interpolate( prev->duck , cur->duck , lerp );

							if ( cur->choked - 1 == i )
							{
								e->velocity( ) = vecVelocity;
								e->flags( ) = cur->flags;

							}
							else // compute velocity and flags.
							{
								cur->vel = ( cur->orig - prev->orig ) * ( 1.f / ticks_to_time( cur->choked ) );
								e->flags( ) = prev->flags;
							}

							//e->get_animstate( )->feet_cycle = cur->layers[ 6 ].cycle;
							//e->get_animstate( )->feet_yaw_rate = cur->layers[ 6 ].weight;
							update_player( e , simtime );
						}
					}
				}
			}
			else
				update_player( e , e->simtime( ) );

			setup_bones( e , bone_used_by_anything & ~bone_used_by_attachment , interfaces::global_vars->curtime , cur->bones );

			e->velocity( ) = e->abs_velocity( ) = o_rec.vel;
			e->flags( ) = o_rec.flags;
			e->set_abs_origin( o_rec.abs_orig );
			e->eflags( ) = o_rec.eflags;
			e->duck_amount( ) = o_rec.duck;
			e->SetAnimLayers( o_rec.layers );
			e->SetPoseParameters( o_rec.poses );
			e->lower_body_yaw( ) = o_rec.lby;
			e->origin( ) = o_rec.orig;
			//e->setup_bones( bones , 128 , bone_used_by_anything & ~bone_used_by_attachment , interfaces::global_vars->curtime );
		}

		void update_ticks( )
		{
			if ( !g::local )
			{
				if ( rec->size( ) )
					rec->clear( );
				return;
			}

			for ( int i = 1; i <= 64; i++ ) {

				auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );

				bool reset = ( !e || !e->is_alive( ) || !e->is_player( ) );

				// if this happens, delete all the lagrecords.
				if ( reset ) {
					if ( rec[ i ].size( ) )
						rec[ i ].clear( );
					continue;
				}
				if ( e == g::local )
					continue;
				if ( e->team( ) == g::local->team( ) )
				{
					e->client_animations( ) = true;
					if ( rec[ i ].size( ) )
						rec[ i ].clear( );
					continue;
				}

				bool update = ( rec[ i ].empty( ) || e->simtime( ) > rec[ i ].front( ).simtime );

				if ( !update && !e->dormant( ) && e->origin( ) != e->old_origin( ) ) {
					update = true;

					// fix data.
					e->simtime( ) = ticks_to_time( interfaces::client_state->clock_drift_mgr.m_server_tick );
				}


				if ( e->dormant( ) ) {
					bool insert = true;

					// we have any records already?
					if ( !rec[ i ].empty( ) ) {

						records* front = &rec[ i ].front( );

						// we already have a dormancy separator.
						if ( front->dormant )
							insert = false;
					}

					if ( insert ) {
						rec[ i ].emplace_front( records( e ) );

						// get reference to newly added record.
						records* current = &rec[ i ].front( );

						// mark as dormant.
						current->dormant = true;
					}
				}
				if ( update )
				{
					rec[ i ].emplace_front( records( e ) );

					//e->simtime( ) = ticks_to_time( interfaces::client_state->clock_drift_mgr.m_server_tick );
					auto current = &rec[ i ].front( );
					current->dormant = false;
					//create_fake_matrix( e , current , current->left , 120 );
					//create_fake_matrix( e , current , current->right , -120 );
					update_anims( e , current );
				}

				while ( rec[ i ].size( ) >= 32 )
					rec[ i ].pop_back( );

				for ( auto tick : rec[ i ] )
				{
					if ( !is_valid( tick.simtime ) )
						rec[ i ].pop_back( );
				}
			}
		}
	}
}