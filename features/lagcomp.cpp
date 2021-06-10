#include <features.h>


bool HandleBoneSetup( base_entity* target , matrix_t* pBoneToWorldOut , int boneMask , float currentTime )
{
	auto hdr = target->get_model_ptr( );
	if ( !hdr )
		return false;

	auto oldBones = target->GetBoneArrayForWrite( );
	const auto o_abs = target->abs_angles( );
	const auto o_origin = target->abs_origin( );
	animlayer layers[ 13 ];
	target->GetAnimLayers( layers );
	float poses[ 24 ];
	target->GetPoseParameters( poses );

	matrix_t baseMatrix;
	math::AngleMatrix( target->abs_angles( ) , target->abs_origin( ) , baseMatrix );

	target->effects( ) |= 0x008;

	ik_context* IK_context = target->get_ik_context( );
	if ( IK_context )
	{
		IK_context->clear_targets( );
		IK_context->init( hdr , target->abs_angles( ) , target->origin( ) ,
			currentTime , interfaces::global_vars->framecount , bone_used_by_server );
	}

	vec3_t pos[ 128 ]{};
	quaternion q[ 128 ]{};
	uint8_t boneComputed[ 256 ]{};

	target->GetBoneArrayForWrite( ) = pBoneToWorldOut;

	target->StandardBlendingRules( hdr , pos , q , currentTime , boneMask );


	if ( IK_context )
	{
		target->update_ik_locks( currentTime );
		IK_context->update_targets( pos , q , pBoneToWorldOut , &boneComputed[ 0 ] );
		target->calculate_ik_locks( currentTime );
		IK_context->solve_dependencies( pos , q , pBoneToWorldOut , &boneComputed[ 0 ] );
	}

	target->BuildTransformations( hdr , pos , q , baseMatrix , boneMask , &boneComputed[ 0 ] );

	target->effects( ) &= ~0x008;

	target->SetAnimLayers( layers );
	target->SetPoseParameters( poses );
	//target->set_abs_origin( o_origin );
	//target->set_abs_angles( o_abs );
	target->GetBoneArrayForWrite( ) = oldBones;
	return true;
}

bool setup_bones( base_entity* target , int boneMask , float currentTime , matrix_t* pBoneToWorldOut )
{
	alignas( 16 ) matrix_t bone_out[ 128 ];
	const auto ret = HandleBoneSetup( target , bone_out , boneMask , currentTime );
	memcpy( pBoneToWorldOut , bone_out , sizeof( matrix_t[ 128 ] ) );
	return ret;
}


namespace features
{
	namespace lagcomp {

		void create_fake_matrix( base_entity* pl, records* rec, matrix_t bones [ 128 ], int delta )
		{
			if (!rec)
				return;

			animlayer backup_layers [ 13 ];
			memcpy( backup_layers, pl->anim_layers( ), sizeof( animlayer ) * 13 );

			// Backup Animstate
			animstate backupState;
			auto state = pl->get_animstate( );
			memcpy( &backupState, state, sizeof( animstate ) );
			state->abs_yaw = math::normalize_yaw( pl->angles( ).y + delta );

			pl->update_animations( );

			pl->inval_bone_cache( );

			setup_bones( pl, bone_used_by_anything & ~bone_used_by_attachment, pl->simtime( ), bones );

			memcpy( pl->anim_layers( ), backup_layers, sizeof( animlayer ) * 13 );

			memcpy( state, &backupState, sizeof( animstate ) );
		}
		records* get_records( base_entity* e, bool last )
		{
			if (!e || !e->is_alive( ))
				return nullptr;
			if (!last)
			{
				for (auto it = rec [ e->idx( ) ].begin( ); it != rec [ e->idx( ) ].end( ); it = next( it ))
				{
					if (is_valid( ( &*it )->simtime ) && !( &*it )->dormant && ( &*it )->valid)
						return ( &*it );
				}
			}
			else
			{
				for (auto it = rec [ e->idx( ) ].rbegin( ); it != rec [ e->idx( ) ].rend( ); it = next( it ))
				{
					if (is_valid( ( &*it )->simtime ) && !( &*it )->dormant && ( &*it )->valid)
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
			if (!cl_interp)
				cl_interp = interfaces::cvar->find_var( "cl_interp" );
			if (!cl_interp_r)
				cl_interp_r = interfaces::cvar->find_var( "cl_interp_ratio" );

			if (!cl_updaterate)
				cl_updaterate = interfaces::cvar->find_var( "cl_updaterate" );

			return max( cl_interp->get_float( ), cl_interp_r->get_float( ) / cl_updaterate->get_float( ) );
		}

		bool is_valid( float simtime )
		{
			const auto channel_info = interfaces::engine->get_net_channel_info( );

			if (!channel_info)
				return false;
			static auto sv_maxunlag = interfaces::cvar->find_var( "sv_maxunlag" );

			float max_unlag = 0.2f;

			if (sv_maxunlag)
				max_unlag = sv_maxunlag->get_float( );

			float correct = channel_info->get_latency( 1 ) + channel_info->get_latency( 0 );
			std::clamp( correct, 0.f, max_unlag );

			return std::fabs( correct - ( antiaim::server_curtime( ) - simtime ) ) <= max_unlag;
		}

		void update_player( base_entity* pl, float sim )
		{
			if (pl == g::local)
				return;
			auto state = pl->get_animstate( );
			if (!state)
				return;

			const float simtime = pl->simtime( );
			const float curtime = interfaces::global_vars->curtime;
			const float frametime = interfaces::global_vars->frametime;
			pl->simtime( ) = sim;

			interfaces::global_vars->curtime = pl->simtime( );
			interfaces::global_vars->frametime = interfaces::global_vars->interval_per_tick;

			pl->inval_bone_cache( );

			if (state->last_clientside_anim_framecount == interfaces::global_vars->framecount)
				state->last_clientside_anim_framecount -= 1;

			/*animations::enable_bones = */pl->client_animations( ) = true;
			pl->get_animstate( )->update( vec3_t( pl->angles().x, pl->get_animstate( )->abs_yaw, pl->angles( ).z ) );
			pl->update_animations( );
			/*animations::enable_bones = */pl->client_animations( ) = false;

			pl->invalidate_physics( 0x2A );
			pl->simtime( ) = simtime;
			interfaces::global_vars->curtime = curtime;
			interfaces::global_vars->frametime = frametime;
		}

		void __declspec( naked ) feet_wobble_fix( )
		{
			__asm
			{
				push edi
				xchg dword ptr [ esp ], edi
				push eax
				mov eax, 77
				mov eax, dword ptr [ esp ]
				add esp, 4
				pop edi

				cmp esp, 0
				jne fixentity

				_emit 0x88
				_emit 0xFF

				invlpg dword ptr [ eax ]

				int 2

				fixentity:
				sub esp, 4
					mov dword ptr [ esp ], ebp

					call cleanup

					pop ebp

					ret

					cleanup :

				ret
			}
		}
		void BuildBones( base_entity* player, matrix_t bones [ 128 ], records* rec ) {
			// keep track of old occlusion values
			const auto backup_occlusion_flags = player->m_iOcclusionFlags( );
			const auto backup_occlusion_framecount = player->m_iOcclusionFramecount( );

			// skip occlusion checks in c_cs_player::setup_bones.
			if (player != g::local)
			{
				player->m_iOcclusionFlags( ) = 0;
				player->m_iOcclusionFramecount( ) = 0;
			}

			// clear bone accessor.
			player->GetReadableBones( ) = player->GetWriteableBones( ) = 0;

			// invalidate bone cache.
			player->GetMostRecentModelBoneCounter( ) = 0;
			player->GetLastBoneSetupTime( ) = -FLT_MAX;

			// stop interpolation.
			player->effects( ) |= 8;

			// build bones.
			setup_bones( player, bone_used_by_anything & ~bone_used_by_attachment, rec->simtime, bones );

			// restore original occlusion.
			if (player != g::local)
			{
				player->m_iOcclusionFlags( ) = backup_occlusion_flags;
				player->m_iOcclusionFramecount( ) = backup_occlusion_framecount;
			}

			// start interpolation again.
			player->effects( ) &= ~8;
		}
		void update_anims( base_entity* e, records* cur )
		{
			records o_rec( e );

			player_info_t info;
			interfaces::engine->get_player_info( e->idx( ), &info );
			bool update_anims = false;

			if (!info.fakeplayer)
			{

				vec3_t vecVelocity = e->velocity( );
				if (rec [ e->idx( ) ].size( ) >= 2)
				{
					auto prev = &rec [ e->idx( ) ][ 1 ];
					if (prev)
					{
						auto velocity = e->velocity( );
						auto was_in_air = e->flags( ) & fl_onground && prev->flags & fl_onground;

						auto time_difference = max( interfaces::global_vars->interval_per_tick, e->simtime( ) - prev->simtime );
						auto origin_delta = e->origin( ) - prev->orig;

						auto animation_speed = 0.0f;
						if (origin_delta != vec3_t( ) && time_to_ticks( time_difference ) > 0)
						{
							e->velocity( ) = origin_delta * ( 1.0f / time_difference );

							if (e->flags( ) & fl_onground && cur->layers [ 11 ].weight > 0.0f && cur->layers [ 11 ].weight < 1.0f && cur->layers [ 11 ].cycle > prev->layers [ 11 ].cycle)
							{
								auto weapon = e->weapon( );

								if (weapon)
								{
									auto max_speed = 260.0f;
									auto weapon_info = weapon->weapon_data( );
									if (weapon_info)
										max_speed = weapon->zoom( ) > 0 ? weapon_info->max_speed_alt : weapon_info->max_speed;

									auto modifier = 0.35f * ( 1.0f - cur->layers [ 11 ].weight );

									if (modifier > 0.0f && modifier < 1.0f)
										animation_speed = max_speed * ( modifier + 0.55f );
								}
							}

							if (animation_speed > 0.0f)
							{
								animation_speed /= e->velocity( ).length_2d( );

								e->velocity( ).x *= animation_speed;
								e->velocity( ).y *= animation_speed;
							}

							if (rec [ e->idx( ) ].size( ) >= 3 && time_difference > interfaces::global_vars->interval_per_tick)
							{
								auto previous_velocity = ( prev->orig - rec [ e->idx( ) ][ 2 ].orig ) * ( 1.0f / time_difference );

								if (previous_velocity != vec3_t( ) && !was_in_air)
								{
									auto current_direction = math::normalize_yaw( rad2deg( atan2( e->velocity( ).y, e->velocity( ).x ) ) );
									auto previous_direction = math::normalize_yaw( rad2deg( atan2( previous_velocity.y, previous_velocity.x ) ) );

									auto average_direction = current_direction - previous_direction;
									average_direction = deg2rad( math::normalize_yaw( current_direction + average_direction * 0.5f ) );

									auto direction_cos = cos( average_direction );
									auto dirrection_sin = sin( average_direction );

									auto velocity_speed = e->velocity( ).length_2d( );

									e->velocity( ).x = direction_cos * velocity_speed;
									e->velocity( ).y = dirrection_sin * velocity_speed;
								}
							}

							if (!( e->flags( ) & fl_onground ))
							{
								static auto sv_gravity = interfaces::cvar->find_var( "sv_gravity" );

								auto fixed_timing = std::clamp < float >( time_difference, interfaces::global_vars->interval_per_tick, 1.0f );
								e->velocity( ).z -= sv_gravity->get_float( ) * fixed_timing * 0.5f;
							}
							else
								e->velocity( ).z = 0.0f;
						}
					}

					e->eflags( ) &= ~efl_dirty_absvelocity;
					if (e->flags( ) & fl_onground && e->velocity( ).length( ) > 0.0f && cur->layers [ 6 ].weight <= 0.0f)
						e->velocity( ) = vec3_t( );

					e->abs_velocity( ) = e->velocity( );

					auto land_time = 0.0f;
					auto land_in_cycle = false;
					auto is_landed = false;
					auto on_ground = false;

					e->SetAnimLayers( prev->layers );
					auto ticks_chocked = 1;
					auto simulation_ticks = time_to_ticks( e->simtime( ) - prev->simtime );
					if (simulation_ticks > 0 && simulation_ticks <= 62)
						ticks_chocked = simulation_ticks;

					if (ticks_chocked > 1)
					{
						if (cur->layers [ 4 ].cycle < 0.5f && ( !( e->flags( ) & fl_onground ) || !( prev->flags & fl_onground ) ))
						{
							land_time = e->simtime( ) - cur->layers [ 4 ].playback_rate * cur->layers [ 4 ].cycle;
							land_in_cycle = land_time >= prev->simtime;
						}

						for (int i = 0; i < ticks_chocked; ++i)
						{
							float simtime = prev->simtime + ticks_to_time( i + 1 );
							const auto lerp = 1.f - ( cur->simtime - simtime ) / ( cur->simtime - prev->simtime );
							e->duck_amount( ) = math::interpolate( prev->duck, cur->duck, lerp );

							on_ground = e->flags( ) & fl_onground;

							if (land_in_cycle && !is_landed)
							{
								if (land_time <= simtime)
								{
									is_landed = true;
									on_ground = true;
								}
								else
									on_ground = prev->flags & fl_onground;
							}

							if (on_ground)
								e->flags( ) |= fl_onground;
							else
								e->flags( ) &= ~fl_onground;

							//e->get_animstate( )->feet_yaw_rate = 0;
							update_anims = true;
							if (update_anims)
								update_player( e, simtime );
						}
					}
				}

			}
			else
				update_player( e, e->simtime( ) );

			if (!update_anims)
			{
				update_player( e, e->simtime( ) );
			}

			setup_bones( e, bone_used_by_anything & ~bone_used_by_attachment, interfaces::global_vars->curtime, cur->bones );
			//	BuildBones( e , cur->bones , cur );

			e->velocity( ) = e->abs_velocity( ) = o_rec.vel;
			e->flags( ) = o_rec.flags;
			e->set_abs_origin( o_rec.abs_orig );
			e->eflags( ) = o_rec.eflags;
			e->duck_amount( ) = o_rec.duck;
			e->SetAnimLayers( o_rec.layers );
			//e->SetPoseParameters( o_rec.poses );
			e->lower_body_yaw( ) = o_rec.lby;
			e->origin( ) = o_rec.orig;
			//e->setup_bones( bones , 128 , bone_used_by_anything & ~bone_used_by_attachment , interfaces::global_vars->curtime );

		}

		void update_ticks(  )
		{
			
		

				if (!g::local)
				{
					for (int i = 0; i <= 64; i++)
					{
						if (rec [ i ].size( ) <= rec [ i ].max_size( ) && rec [ i ].size( ) > 0)
							rec [ i ].clear( );
					}
					return;
				}

				for (int i = 1; i <= 64; i++) {

					auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
					if (e == g::local)
						continue;
					if (!e || !e->is_alive( ) || !e->is_player( )) {
						if (rec [ i ].size( ) <= rec [ i ].max_size( ) && rec [ i ].size( ) > 0 )
							rec [ i ].clear( );
						continue;
					}
					if (e->team( ) == g::local->team( ))
					{
						e->client_animations( ) = true;
						if (rec [ i ].size( ) <= rec [ i ].max_size( ) && rec [ i ].size( ) > 0)
							rec [ i ].clear( );
						continue;
					}



					auto time_delta = abs( time_to_ticks( e->simtime( ) ) - interfaces::global_vars->tickcount );

					if (time_delta > 1.0f / interfaces::global_vars->interval_per_tick)
						continue;

					if (e->dormant( )) {
						bool insert = true;

						if (!rec [ i ].empty( )) {

							records* front = &rec [ i ].front( );

							if (front->dormant)
								insert = false;
						}

						if (insert) {
							rec [ i ].emplace_front( records( e ) );

							records* current = &rec [ i ].front( );

							current->dormant = true;
						}
					}


					auto update = rec [ i ].empty( ) || e->simtime( ) != e->old_simtime( );
					auto server_tick = interfaces::client_state->clock_drift_mgr.m_server_tick - i % interfaces::global_vars->m_timestamprandomizewindow;
					auto current_tick = server_tick - server_tick % interfaces::global_vars->m_timestampnetworkingbase;

					if (update && !rec [ i ].empty( ))
					{

						if (time_to_ticks( e->old_simtime( ) ) < current_tick && time_to_ticks( e->simtime( ) ) == current_tick)
						{
							auto layer = &e->anim_layers( ) [ 11 ];
							auto previous_layer = &rec [ i ].front( ).layers [ 11 ];

							if (layer->cycle == previous_layer->cycle)
							{
								e->simtime( ) = e->old_simtime( );
								update = false;
							}
						}
					}

					if (!update && e->origin( ) != e->old_origin( ))
					{
						e->simtime( ) = ticks_to_time( server_tick );
						update = true;
					}
					try
					{
						if (update)
						{
							rec [ i ].emplace_front( records( e ) );

							//e->simtime( ) = ticks_to_time( interfaces::client_state->clock_drift_mgr.m_server_tick );
							auto current = &rec [ i ].front( );
							current->dormant = false;

							/*create_fake_matrix( e , current , current->left , 120 );
							create_fake_matrix( e , current , current->right , -120 );*/
							update_anims( e, current );
						}
					}
					catch (const std::runtime_error& error)
					{
						MessageBoxA( NULL, error.what( ), "DEBUG", MB_OK | MB_ICONERROR );
					}
					for (auto tick : rec [ i ])
					{
						if (!is_valid( tick.simtime ))
							rec [ i ].pop_back( );
					}
				}
			
			
		}
	}
}