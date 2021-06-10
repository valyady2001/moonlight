#include "features.h"

int get_tickbase( ) {

	static int tick = 0;
	static usercmd* last_cmd = nullptr;

	if ( g::cmd != nullptr )
	{
		// if command was not predicted - increment tickbase
		if ( last_cmd == nullptr || last_cmd->hasbeenpredicted )
			tick = g::local->tick_base( );
		else
			tick++;

		last_cmd = g::cmd;
	}

	return tick;
}

bool build_matrix( base_entity* pl , matrix_t* out , int max_bones , int mask , float seed ) {
	const auto backup_mask_1 = *reinterpret_cast< int* >( uintptr_t( pl ) + 0x269C );
	const auto backup_mask_2 = *reinterpret_cast< int* >( uintptr_t( pl ) + 0x26B0 );
	const auto backup_flags = *reinterpret_cast< int* >( uintptr_t( pl ) + 0xe8 );
	const auto backup_effects = *reinterpret_cast< int* >( uintptr_t( pl ) + 0xf0 );
	const auto backup_use_pred_time = *reinterpret_cast< int* >( uintptr_t( pl ) + 0x2ee );
	auto backup_abs_origin = pl->abs_origin( );

	*reinterpret_cast< int* >( uintptr_t( pl ) + 0xA68 ) = 0;
	const auto backup_frametime = interfaces::global_vars->frametime;

	interfaces::global_vars->frametime = FLT_MAX;

	*reinterpret_cast< int* >( uintptr_t( pl ) + 0x269C ) = 0;
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0x26B0 ) |= 512;
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0xe8 ) |= 8;

	/* disable matrix interpolation */
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0xf0 ) |= 8;

	/* use our setup time */
	*reinterpret_cast< bool* >( uintptr_t( pl ) + 0x2ee ) = false;

	/* thanks chambers */
	auto b = *reinterpret_cast< char* > ( uintptr_t( pl ) + 0x68 );

	auto backup_num_ik_chains = 0;

	*reinterpret_cast< char* > ( uintptr_t( pl ) + 0x68 ) |= 2;

	/* use uninterpolated origin */
	pl->inval_bone_cache( );
	pl->set_abs_origin( pl->origin( ) );

	const auto ret = pl->setup_bones( out , max_bones , mask , seed );
	*reinterpret_cast< char* > ( uintptr_t( pl ) + 0x68 ) = b;
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0x269C ) = backup_mask_1;
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0x26B0 ) = backup_mask_2;
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0xe8 ) = backup_flags;
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0xf0 ) = backup_effects;
	*reinterpret_cast< int* >( uintptr_t( pl ) + 0x2ee ) = backup_use_pred_time;

	interfaces::global_vars->frametime = backup_frametime;
	pl->set_abs_origin( backup_abs_origin );

	return ret;
}

template<class T , class U>
inline T clamp( T in , U low , U high )
{
	if ( in <= low )
		return low;
	else if ( in >= high )
		return high;
	else
		return in;
}


void FixJumpFallPose( base_entity* pEntity , animstate* pState )
{
	if ( !pEntity || !pState || !pEntity->anim_layers( ) ) {
		return;
	}

	float flAirTime = ( pEntity->anim_layers( )[ 4 ] ).cycle;
	const float AirTime = ( flAirTime - 0.72f ) * 1.25f;
	const float Clamped = AirTime >= 0.f ? std::min< float >( AirTime , 1.f ) : 0.f;
	float JumpFallTime = ( 3.f - ( Clamped + Clamped ) ) * ( Clamped * Clamped );

	if ( JumpFallTime >= 0.f )
		JumpFallTime = std::min< float >( JumpFallTime , 1.f );

	pState->jump_fall_pose.set_value( pEntity , JumpFallTime );
}


void FixMoveBlendPose( base_entity* pEntity , animstate* pState )
{
	if ( !pEntity || !pState || !pEntity->anim_layers( ) ) {
		return;
	}

	pState->move_blend_walk_pose.set_value( pEntity , ( 1.0f - pState->ground_fraction ) * ( 1.0f - pState->duck_amount ) );
	pState->move_blend_run_pose.set_value( pEntity , ( pState->ground_fraction ) * ( 1.0f - pState->duck_amount ) );
	pState->move_blend_crouch_pose.set_value( pEntity , pState->duck_amount );
}


#include <csgo.h>
namespace features
{
	namespace animations
	{
		float jump_fall( base_entity* pl , float air_time )
		{
			const float airtime = ( air_time - 0.72f ) * 1.25f;
			const float clamped = airtime >= 0.f ? std::min< float >( airtime , 1.f ) : 0.f;
			float jump_fall = ( 3.f - ( clamped + clamped ) ) * ( clamped * clamped );

			if ( jump_fall >= 0.f )
				jump_fall = std::min< float >( jump_fall , 1.f );

			return jump_fall;
		}

#pragma region local
		void update_local( )
		{
			if ( !g::local || !g::local->is_alive( ) || !g::cmd )
				return;

			auto state = g::local->get_animstate( );
			if ( !state )
				return;

			const auto backup_frametime = interfaces::global_vars->frametime;
			const auto backup_realtime = interfaces::global_vars->realtime;
			const auto backup_eflags = g::local->eflags( );

			interfaces::global_vars->frametime = FLT_MAX;

			constexpr auto pose_jump_fall = [ ]( base_entity* ent , float air_time ) {
				auto airtime = ( air_time - 0.72f ) * 1.25f;
				auto clamped = airtime >= 0.0f ? std::min< float >( airtime , 1.0f ) : 0.0f;
				auto jump_fall = ( 3.0f - ( clamped + clamped ) ) * ( clamped * clamped );

				if ( jump_fall >= 0.0f )
					jump_fall = std::min< float >( jump_fall , 1.0f );

				ent->poses( )[ 6 ] = jump_fall;
			};

			if ( !state->on_ground )
				pose_jump_fall( g::local , g::local->anim_layers( )[ 4 ].cycle );

			g::local->anim_layers( )[ 12 ].weight = 0.f;

			if ( _realstate.update )
			{
				std::memcpy( _realstate.layers.data( ) , g::local->anim_layers( ) , sizeof( animlayer ) * 15 );

				g::local->eflags( ) &= ~4096; /* efl_dirty_absvelocity */
				g::local->abs_velocity( ) = g::local->velocity( );

				if ( state->hit_ground )
					state->time_in_air = 0.0f;

				state->last_clientside_anim_framecount -= 1;
				animations::enable_bones= g::local->client_animations( ) = true;
				state->update( g::sendpacket ? g::sent_angle : g::choked_angle );
				g::local->update_animations( );

				if ( g::sendpacket )
				{
					_realstate.proper_abs = state->abs_yaw;
					std::memcpy( _realstate.sent_poses.data( ) , g::local->poses( ).data( ) , sizeof( float ) * 24 );
				}

				_realstate.update = false;

			}

			animations::enable_bones = g::local->client_animations( ) = false;


			g::local->set_abs_angles( vec3_t( 0 , _realstate.proper_abs , 0 ) );

			interfaces::global_vars->frametime = backup_frametime;
			g::local->eflags( ) = backup_eflags;

			std::memcpy( g::local->anim_layers( ) , _realstate.layers.data( ) , sizeof( animlayer ) * 15 );
			std::memcpy( g::local->poses( ).data( ) , _realstate.sent_poses.data( ) , sizeof( float ) * 24 );

		}
		void update_desync( )
		{
			if ( !g::local || !g::local->is_alive( ) || !g::cmd )
				return;

			if ( fake_state.m_fake_spawntime != g::local->spawn_time( ) || fake_state.m_should_update_fake )
			{
				fake_state.init_fake_anim = false;
				fake_state.m_fake_spawntime = g::local->spawn_time( );
				fake_state.m_should_update_fake = false;
			}

			if ( !fake_state.init_fake_anim )
			{
				fake_state.m_fake_state = static_cast< animstate* > ( interfaces::mem_alloc->alloc( sizeof( animstate ) ) );

				if ( fake_state.m_fake_state != nullptr )
					g::local->create_animstate( fake_state.m_fake_state );

				fake_state.init_fake_anim = true;
			}
			if ( !g::sendpacket )
				return;

			if ( fake_state.m_fake_state )
			{
				g::local->anim_layers( )[ 12 ].weight = FLT_EPSILON;

				animlayer o_overlays[ 13 ];
				float o_poses[ 20 ];
				//setup_lean( g::local , fake_state.m_fake_state );
				std::memcpy( o_overlays , g::local->anim_layers( ) , sizeof( o_overlays ) );
				std::memcpy( o_poses , g::local->poses( ).data( ) , sizeof( o_poses ) );

				auto backup_abs_angles = g::local->abs_angles( );
				const auto backup_frametime = interfaces::global_vars->frametime;

				interfaces::global_vars->frametime = FLT_MAX;

				fake_state.m_fake_state->update( g::sent_angle );

				fake_state.m_fake_rotation = fake_state.m_fake_state->abs_yaw;

				fake_state.m_got_fake_matrix = build_matrix( g::local , fake_state.m_fake_matrix , 128 , bone_used_by_anything & ~bone_used_by_attachment , interfaces::global_vars->curtime );

				const auto org_tmp = g::local->render_origin( );

				if ( fake_state.m_got_fake_matrix )
				{
					for ( auto& i : fake_state.m_fake_matrix )
					{
						i[ 0 ][ 3 ] -= org_tmp.x;
						i[ 1 ][ 3 ] -= org_tmp.y;
						i[ 2 ][ 3 ] -= org_tmp.z;
					}
				}

				std::memcpy( fake_state.anim_layers.data( ) , g::local->anim_layers( ) , sizeof( fake_state.anim_layers ) );
				std::memcpy( fake_state.pose_params.data( ) , g::local->poses( ).data( ) , sizeof( fake_state.pose_params ) );

				g::local->set_abs_angles( vec3_t( 0.f , backup_abs_angles.y , 0.f ) );
				interfaces::global_vars->frametime = backup_frametime;

				std::memcpy( g::local->anim_layers( ) , o_overlays , sizeof( o_overlays ) );
				std::memcpy( g::local->poses( ).data( ) , o_poses , sizeof( o_poses ) );
			}
		}
#pragma endregion

		void extrapolate( base_entity* pEntity , vec3_t& vecOrigin , vec3_t& vecVelocity , int& fFlags , bool bOnGround )
		{
			vec3_t                start , end , normal;
			trace_t            trace;
			trace_world_only filter;
			ray_t ray;

			// define trace start.
			start = vecOrigin;

			// move trace end one tick into the future using predicted velocity.
			end = start + ( vecVelocity * interfaces::global_vars->interval_per_tick );

			// trace.
			ray.initialize( start , end , pEntity->collideable( )->mins( ) , pEntity->collideable( )->maxs( ) );
			interfaces::trace_ray->trace_ray( ray , contents_solid , &filter , &trace );

			// we hit shit
			// we need to fix shit.
			if ( trace.flFraction != 1.f ) {

				// fix sliding on planes.
				for ( int i{ }; i < 2; ++i ) {
					vecVelocity -= trace.plane.normal * vecVelocity.dot( trace.plane.normal );

					float adjust = vecVelocity.dot( trace.plane.normal );
					if ( adjust < 0.f )
						vecVelocity -= ( trace.plane.normal * adjust );

					start = trace.end;
					end = start + ( vecVelocity * ( interfaces::global_vars->interval_per_tick * ( 1.f - trace.flFraction ) ) );

					ray.initialize( start , end , pEntity->collideable( )->mins( ) , pEntity->collideable( )->maxs( ) );
					interfaces::trace_ray->trace_ray( ray , contents_solid , &filter , &trace );
					if ( trace.flFraction == 1.f )
						break;
				}
			}

			// set new final origin.
			start = end = vecOrigin = trace.end;

			// move endpos 2 units down.
			// this way we can check if we are in/on the ground.
			end.z -= 2.f;

			// trace.
			ray.initialize( start , end , pEntity->collideable( )->mins( ) , pEntity->collideable( )->maxs( ) );
			interfaces::trace_ray->trace_ray( ray , contents_solid , &filter , &trace );

			// strip onground flag.
			fFlags &= ~fl_onground;

			// add back onground flag if we are onground.
			if ( trace.flFraction != 1.f && trace.plane.normal.z > 0.7f )
				fFlags |= fl_onground;
		}

	}
}