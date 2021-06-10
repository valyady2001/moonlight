#include <features.h>
#include <render/render.h>
#define square( x ) ( x * x )

namespace features
{
	namespace misc
	{
#pragma region tp
		void thirdperson( ) {
			bool in_tp = keyhandler::auto_check( g_var.misc.tp_bind[ 1 ] , g_var.misc.tp_bind[ 0 ] );

			auto get_ideal_dist = [ & ]( float ideal_distance ) {
				vec3_t inverse = interfaces::engine->get_view_angles( );

				inverse.x *= -1.0f , inverse.y += 180.0f;

				vec3_t direction = math::angle_1vec( inverse );

				ray_t ray;
				trace_t trace;
				trace_filter filter;
				filter.skip = g::local;
				ray.initialize( g::local->eye_pos( ) , g::local->eye_pos( ) + ( direction * ideal_distance ) );
				interfaces::trace_ray->trace_ray( ray , mask_solid & ~contents_monster , &filter , &trace );
				if ( trace.did_hit( ) && trace.DidHitNonWorldEntity( ) )
				{
					if ( trace.entity->idx( ) != -1 && trace.entity->is_player( ) ) // aka it hit a valid player. returns -1 if it didnt
					{
						return  ideal_distance - 10.f;
					}
					else
						return ( ideal_distance * trace.flFraction ) - 10.f;
				}
				else
					return ( ideal_distance * trace.flFraction ) - 10.f;
			};

			if ( in_tp ) {
				interfaces::input->camera_in_thirdperson = true;
				vec3_t ang = interfaces::engine->get_view_angles( );
				interfaces::input->camera_offset = vec3_t( ang.x , ang.y , get_ideal_dist( g_var.esp.camdist ) );
			}
			else {
				interfaces::input->camera_in_thirdperson = false;
			}
		}

#pragma endregion

#pragma region grenadepred
		void predict( usercmd* user_cmd ) {
			if ( !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) )
				return;

			//	readability
			constexpr float restitution = 0.45f;
			constexpr float power[ ] = { 1.0f, 1.0f, 0.5f, 0.0f };
			constexpr float velocity = 403.0f * 0.9f;

			float step , gravity , new_velocity , unk01;
			int index{} , grenade_act{ 1 };
			vec3_t pos , thrown_direction , start , eye_origin;
			vec3_t angles , thrown;

			//	first time setup
			static auto sv_gravity = interfaces::cvar->find_var( "sv_gravity" );

			//	calculate step and actual gravity value
			gravity = sv_gravity->get_float( ) / 8.0f;
			step = interfaces::global_vars->interval_per_tick;

			//	get local view and eye origin
			eye_origin = g::local->eye_pos( );
			angles = interfaces::engine->get_view_angles( );

			//	copy current angles and normalise pitch
			thrown = angles;

			if ( thrown.x < 0 ) {
				thrown.x = -10 + thrown.x * ( ( 90 - 10 ) / 90.0f );
			}
			else {
				thrown.x = -10 + thrown.x * ( ( 90 + 10 ) / 90.0f );
			}

			//	find out how we're throwing the grenade
			auto primary_attack = user_cmd->buttons & in_attack;
			auto secondary_attack = user_cmd->buttons & in_attack2;

			if ( primary_attack && secondary_attack ) {
				grenade_act = ACT_LOB;
			}
			else if ( secondary_attack ) {
				grenade_act = ACT_DROP;
			}

			//	apply 'magic' and modulate by velocity
			unk01 = power[ grenade_act ];

			unk01 = unk01 * 0.7f;
			unk01 = unk01 + 0.3f;

			new_velocity = velocity * unk01;

			//	here's where the fun begins
			math::angle_2vectors( thrown , &thrown_direction );

			start = eye_origin + thrown_direction * 16.0f;
			thrown_direction = ( thrown_direction * new_velocity ) + g::local->velocity( );

			//	let's go ahead and predict
			for ( auto time = 0.0f; index < 500; time += step ) {

				pos = start + thrown_direction * step;
				trace_t trace;
				ray_t ray;
				trace_filter_skip_one_entity filter( g::local );

				ray.initialize( start , pos );
				interfaces::trace_ray->trace_ray( ray , mask_solid , &filter , &trace );

				//	modify path if we have hit something
				if ( trace.flFraction != 1.0f ) {
					thrown_direction = trace.plane.normal * -2.0f * thrown_direction.dot( trace.plane.normal ) + thrown_direction;

					thrown_direction *= restitution;

					pos = start + thrown_direction * trace.flFraction * step;

					time += ( step * ( 1.0f - trace.flFraction ) );
				}

				//	check for detonation
				auto detonate = detonated( g::local->weapon( ) , time , trace );

				//	emplace nade point
				_points.at( index++ ) = c_nadepoint( start , pos , trace.flFraction != 1.0f , true , trace.plane.normal , detonate );
				start = pos;

				//	apply gravity modifier
				thrown_direction.z -= gravity * trace.flFraction * step;

				if ( detonate ) {
					break;
				}
			}

			//	invalidate all empty points and finish prediction
			for ( auto n = index; n < 500; ++n ) {
				_points.at( n ).m_valid = false;
			}

			_predicted = true;
		}

		bool detonated( c_weapon* weapon , float time , trace_t& trace ) {

			if ( !weapon )
				return true;

			//	get weapon item index
			const auto index = weapon->item_definition_index( );

			switch ( index ) {
				//	flash and HE grenades only live up to 2.5s after thrown
			case item_def_idx::hegrenade:
			case item_def_idx::flashbang:
				if ( time > 2.5f ) {
					return true;
				}
				break;

				//	fire grenades detonate on ground hit, or 3.5s after thrown
			case item_def_idx::molotov:
			case item_def_idx::inc:
				if ( trace.flFraction != 1.0f && trace.plane.normal.z > 0.7f || time > 3.5f ) {
					return true;
				}
				break;

				//	decoy and smoke grenades were buggy in prediction, so i used this ghetto work-around
			case item_def_idx::decoy:
			case item_def_idx::smokegrenade:
				if ( time > 5.0f ) {
					return true;
				}
				break;
			}

			return false;
		}

		void trace( usercmd* user_cmd ) {
			if ( !interfaces::engine->is_connected( ) ||
				!interfaces::engine->is_in_game( ) ||
				!g_var.esp.grenade_pred )
				return;

			if ( !( user_cmd->buttons & in_attack ) && !( user_cmd->buttons & in_attack2 ) ) {
				_predicted = false;
				return;
			}

			const static std::vector< int > nades{
				item_def_idx::flashbang,
				item_def_idx::smokegrenade,
				item_def_idx::hegrenade,
				item_def_idx::molotov,
				item_def_idx::decoy,
				item_def_idx::inc
			};

			//	grab local weapon
			auto weapon = g::local->weapon( );

			if ( !weapon )
				return;


			if ( std::find( nades.begin( ) , nades.end( ) , weapon->item_definition_index( ) ) != nades.end( ) )
				return predict( user_cmd );


			_predicted = false;
		}

		void draw( ) {
			if ( !interfaces::engine->is_connected( ) ||
				!interfaces::engine->is_in_game( ) ||
				!g_var.esp.grenade_pred ||
				!g::local ||
				!g::local->is_alive( ) )
				return;

			auto draw_3d_dotted_circle = [ ]( vec3_t position , float points , float radius ) {
				float step = 3.141592654f * 2.0f / points;
				for ( float a = 0; a < 3.141592654f * 2.0f; a += step ) {
					vec3_t start( radius * cosf( a ) + position.x , radius * sinf( a ) + position.y , position.z );
					vec3_t end( radius * cosf( a + step ) + position.x , radius * sinf( a + step ) + position.y , position.z );

					vec2_t start2d;
					vec2_t end2d;
					if ( math::w2s( start , start2d ) && math::w2s( end , end2d ) )
						render::line( start2d , end2d , color( int( 0 ) ,
							int( 130 ) ,
							int( 220 ) , 255 ) );
				}
			};

			vec2_t start , end;

			//	draw nade path
			if ( _predicted ) {
				for ( auto& p : _points ) {
					if ( !p.m_valid ) {
						break;
					}

					if ( math::w2s( p.m_start , start ) && math::w2s( p.m_end , end ) ) {

						render::line( start , end , color( int( 255 ) ,
							int( 255 ) ,
							int( 255 ) , 255 ) );

						if ( p.m_detonate || p.m_plane ) {
							;
							render::filled_rect( vec2_t( start.x - 2 , start.y - 2 ) , vec2_t( 5 , 5 ) , color( int( 0 ) ,
								int( 0 ) ,
								int( 255 ) , 255 ) );
						}
						if ( p.m_detonate /*&& g_var.misc.grenade_prediction.show_radius*/ )
							draw_3d_dotted_circle( p.m_end , 40 , 100 );
					}
				}
			}
		}
#pragma endregion

#pragma region movement
		void bunnyhop( usercmd* cmd )
		{
			if ( !g_var.misc.bunnyhop )
				return;

			if ( !g::local ||
				!g::local->is_alive( ) ||
				g::local->move_type( ) == ( int ) move_type::type_noclip ||
				g::local->move_type( ) == ( int ) move_type::type_ladder ||
				!( cmd->buttons & in_jump ) )
				return;

			if ( g::local->flags( ) & fl_onground )
				cmd->buttons |= in_jump;
			else
				cmd->buttons &= ~in_jump;

		}
		void strafer( usercmd* cmd )
		{
			if ( !g_var.misc.strafer )
				return;

			if ( !g::local ||
				!g::local->is_alive( ) ||
				g::local->move_type( ) == ( int ) move_type::type_noclip ||
				g::local->move_type( ) == ( int ) move_type::type_ladder ||
				!( cmd->buttons & in_jump ) )
				return;

			const auto vel = g::local->velocity( );
			const float yaw_velocity = rad2deg( atan2( vel.y , vel.x ) );
			static float sideSpeed = interfaces::cvar->find_var( "cl_sidespeed" )->get_float( );
			vec3_t local_view = interfaces::engine->get_view_angles( );

			if ( !( g::local->flags( ) & fl_onground ) )
			{

				if ( cmd->buttons & in_back )
				{
					g::strafe_angle.y = local_view.y - 180.f;

					if ( GetAsyncKeyState( 'A' ) )
						g::strafe_angle.y = local_view.y + 135.f;
					else if ( GetAsyncKeyState( 'D' ) )
						g::strafe_angle.y = local_view.y - 135.f;
				}
				else if ( GetAsyncKeyState( 'A' ) )
				{
					g::strafe_angle.y = local_view.y + 90.f;

					if ( cmd->buttons & in_back )
						g::strafe_angle.y = local_view.y + 135.f;
					else if ( GetAsyncKeyState( 'W' ) )
						g::strafe_angle.y = local_view.y + 45.f;
				}
				else if ( GetAsyncKeyState( 'D' ) )
				{
					g::strafe_angle.y = local_view.y - 90.f;

					if ( GetAsyncKeyState( 'W' ) )
						g::strafe_angle.y = local_view.y - 45.f;
					else if ( cmd->buttons & in_back )
						g::strafe_angle.y = local_view.y + 135.f;
				}
				else
				{
					g::strafe_angle.y = local_view.y;
				}

				const float diff_ang = math::normalize_yaw( g::strafe_angle.y - yaw_velocity );

				if ( !g::local->velocity( ).length_2d( ) > 0.5 || g::local->velocity( ).length_2d( ) == NAN || g::local->velocity( ).length_2d( ) == INFINITE )
				{
					cmd->forwardmove = sideSpeed;
					return;
				}

				cmd->forwardmove = std::clamp( ( 1.f / g::local->velocity( ).length_2d( ) ) , -sideSpeed , sideSpeed );

				if ( ( cmd->forwardmove < -sideSpeed || cmd->forwardmove > sideSpeed ) )
					cmd->forwardmove = 0;

				cmd->sidemove = ( diff_ang > 0.0 ) ? -sideSpeed : sideSpeed;
				g::strafe_angle.y = math::normalize_yaw( g::strafe_angle.y - diff_ang );

			}

		}
		void fake_duck( usercmd* cmd )
		{
			if ( !g::local->is_alive( ) || !g::local )
				return;

			if ( keyhandler::auto_check( g_var.antihit.fakeduck_bind[ 1 ] , g_var.antihit.fakeduck_bind[ 0 ] ) )
			{
				if ( interfaces::engine->get_net_channel( )->choked_packets <= 7 ) {
					cmd->buttons &= ~in_duck;
				}
				else {
					cmd->buttons |= in_duck;
				}
			}
		}
		void small_movement( )
		{
			if ( !g::local->is_alive( ) || !g::local )
				return;
			if ( !( g::local->flags( ) & fl_onground ) )
				return;

			if ( GetAsyncKeyState( 65 ) || GetAsyncKeyState( 83 ) || GetAsyncKeyState( 68 ) || GetAsyncKeyState( 'W' ) )
				return;

			if ( g::local->velocity( ).length_2d( ) > 4 )
				return;

			static bool flip = false;

			if ( g::sendpacket )
				flip = !flip;

			if ( keyhandler::auto_check( g_var.antihit.fakeduck_bind[ 1 ] , g_var.antihit.fakeduck_bind[ 0 ] ) )
			{
				if ( g::local->flags( ) & fl_onground )
					g::cmd->forwardmove = flip ? 3.f : -3.f;
			}
			else
			{
				if ( g::local->flags( ) & fl_onground  )
					g::cmd->forwardmove = flip ? 1.1f : -1.1f;
			}
		}
		void clamp_move( usercmd* ucmd , float max_speed )
		{
			if ( !g::local || g::local->health( ) <= 0 )
				return;

			/* there is no point of clamping if max speed is negative */
			if ( max_speed <= 0.f ) return;

			/* get speed & same tip as above */
			float m_speed = ( float ) ( std::sqrt( square( ucmd->forwardmove ) + square( ucmd->sidemove ) + square( ucmd->upmove ) ) );
			if ( m_speed <= 0.f ) return;

			/* increase the speed when ducking */
			if ( ucmd->buttons & in_duck ) {
				max_speed *= 2.94117647f;
			}

			/* clamp move if our speed is not higher than the maximum speed we want to move */
			if ( m_speed <= max_speed ) return;

			/* get ratio of speed */
			float m_ratio = max_speed / m_speed;

			ucmd->forwardmove *= m_ratio;
			ucmd->sidemove *= m_ratio;
			ucmd->upmove *= m_ratio;
		}
		void autostop( usercmd* cmd )
		{
			if ( !g::local || !g::local->is_alive( ) || !cmd || !g_var.ragebot.autostop_mode )
				return;

			auto weapon = g::local->weapon( );
			if ( !weapon || weapon->is_knife( ) || weapon->clip1_count( ) <= 0 || weapon->is_reloading( ) ) return;
			if ( weapon->item_definition_index( ) == c4 || weapon->is_grenade( ) ) return;

			if ( g::local->flags( ) & fl_onground ) {
				if ( ragebot::pre_stop )
				{
					if ( g::local->velocity( ).length_2d( ) > 160 )
					{
						vec3_t angle;
						math::vector_angles( g::local->velocity( ) , angle );

						// get our current speed of travel.
						float speed = g::local->velocity( ).length( );

						// fix direction by factoring in where we are looking.
						angle.y = interfaces::engine->get_view_angles( ).y - angle.y;

						// convert corrected angle back to a direction.
						vec3_t direction;
						math::angle_2vectors( angle , &direction );

						vec3_t stop = direction * -speed;

						cmd->forwardmove = stop.x;
						cmd->sidemove = stop.y;
					}
					else
						clamp_move( cmd , 110 );
				}
				if ( ragebot::can_stop )
				{
					switch ( g_var.ragebot.autostop_mode )
					{
					case 1://slide
						if ( g::local->velocity( ).length_2d( ) > g::max_speed - 10 )
						{
							vec3_t angle;
							math::vector_angles( g::local->velocity( ) , angle );

							// get our current speed of travel.
							float speed = g::local->velocity( ).length( );

							// fix direction by factoring in where we are looking.
							angle.y = interfaces::engine->get_view_angles( ).y - angle.y;

							// convert corrected angle back to a direction.
							vec3_t direction;
							math::angle_2vectors( angle , &direction );

							vec3_t stop = direction * -speed;

							cmd->forwardmove = stop.x;
							cmd->sidemove = stop.y;
						}
						else
							clamp_move( cmd , g::max_speed - 15 );

						break;
					case 2://full

						if ( ragebot::can_stop )
						{
							if ( g::local->velocity( ).length_2d( ) > g::max_speed - 10 )
							{
								vec3_t angle;
								math::vector_angles( g::local->velocity( ) , angle );

								// get our current speed of travel.
								float speed = g::local->velocity( ).length( );

								// fix direction by factoring in where we are looking.
								angle.y = interfaces::engine->get_view_angles( ).y - angle.y;

								// convert corrected angle back to a direction.
								vec3_t direction;
								math::angle_2vectors( angle , &direction );

								vec3_t stop = direction * -speed;

								cmd->forwardmove = stop.x;
								cmd->sidemove = stop.y;
							}
							else
								clamp_move( cmd , 20 );
						}
						break;
					}
					ragebot::can_stop = false;
				}
			}
		}
		void fakelag( )
		{

			if ( !g::local || !g::local->is_alive( )
				|| interfaces::engine->is_voice_recording( ) || !g_var.antihit.enable_fl )
			{
				lag_amt = 1;
			}
			else
			{
				if ( g_var.ragebot.exploit > 0 && !keyhandler::auto_check( g_var.antihit.fakeduck_bind[ 1 ] , g_var.antihit.fakeduck_bind[ 0 ] ) && keyhandler::auto_check( g_var.ragebot.exploit_bind[ 1 ] , g_var.ragebot.exploit_bind[ 0 ] ) )
					lag_amt = 1;
				else if ( keyhandler::auto_check( g_var.antihit.fakeduck_bind[ 1 ] , g_var.antihit.fakeduck_bind[ 0 ] ) )
					lag_amt = 14;
				else
				{
					switch ( g_var.antihit.type )
					{
					case 0:
						if ( !( g::local->flags( ) & fl_onground ) )
							lag_amt = g_var.antihit.air_lag;
						else if ( g::local->velocity( ).length( ) > 5 && g::local->flags( ) & fl_onground )
							lag_amt = g_var.antihit.mv_lag;
						else if ( g::local->velocity( ).length( ) < 6 && g::local->flags( ) & fl_onground )
							lag_amt = g_var.antihit.st_lag;
						break;

					case 1:
						lag_amt = math::random_float( g_var.antihit.min_lag , g_var.antihit.max_lag );
						break;
					case 2:
						lag_amt = max( std::clamp( int( std::clamp( g::local->velocity( ).length( ) / interfaces::cvar->find_var( "sv_maxspeed" )->get_float( ) , 0.f , interfaces::cvar->find_var( "sv_maxspeed" )->get_float( ) ) * 14.f ) , 1 , 14 ) , g_var.antihit.adaptive );
						break;
					}
				}
			}

			if ( interfaces::client_state->m_net_channel->choked_packets >= max( 1 , lag_amt ) )
				g::sendpacket = true;
			else
				g::sendpacket = false;

		}
#pragma endregion
#pragma region fixmovement

		void fix_movement( usercmd* cmd , vec3_t wish_angles ) {
			vec3_t view_fwd , view_right , view_up , cmd_fwd , cmd_right , cmd_up;
			math::angle_4vec( wish_angles , &view_fwd , &view_right , &view_up );
			math::angle_4vec( cmd->viewangles , &cmd_fwd , &cmd_right , &cmd_up );

			const auto v8 = sqrtf( ( view_fwd.x * view_fwd.x ) + ( view_fwd.y * view_fwd.y ) );
			const auto v10 = sqrtf( ( view_right.x * view_right.x ) + ( view_right.y * view_right.y ) );
			const auto v12 = sqrtf( view_up.z * view_up.z );

			const vec3_t norm_view_fwd( ( 1.f / v8 ) * view_fwd.x , ( 1.f / v8 ) * view_fwd.y , 0.f );
			const vec3_t norm_view_right( ( 1.f / v10 ) * view_right.x , ( 1.f / v10 ) * view_right.y , 0.f );
			const vec3_t norm_view_up( 0.f , 0.f , ( 1.f / v12 ) * view_up.z );

			const auto v14 = sqrtf( ( cmd_fwd.x * cmd_fwd.x ) + ( cmd_fwd.y * cmd_fwd.y ) );
			const auto v16 = sqrtf( ( cmd_right.x * cmd_right.x ) + ( cmd_right.y * cmd_right.y ) );
			const auto v18 = sqrtf( cmd_up.z * cmd_up.z );

			const vec3_t norm_cmd_fwd( ( 1.f / v14 ) * cmd_fwd.x , ( 1.f / v14 ) * cmd_fwd.y , 0.f );
			const vec3_t norm_cmd_right( ( 1.f / v16 ) * cmd_right.x , ( 1.f / v16 ) * cmd_right.y , 0.f );
			const vec3_t norm_cmd_up( 0.f , 0.f , ( 1.f / v18 ) * cmd_up.z );

			const auto v22 = norm_view_fwd.x * cmd->forwardmove;
			const auto v26 = norm_view_fwd.y * cmd->forwardmove;
			const auto v28 = norm_view_fwd.z * cmd->forwardmove;
			const auto v24 = norm_view_right.x * cmd->sidemove;
			const auto v23 = norm_view_right.y * cmd->sidemove;
			const auto v25 = norm_view_right.z * cmd->sidemove;
			const auto v30 = norm_view_up.x * cmd->upmove;
			const auto v27 = norm_view_up.z * cmd->upmove;
			const auto v29 = norm_view_up.y * cmd->upmove;

			cmd->forwardmove = ( ( ( ( norm_cmd_fwd.x * v24 ) + ( norm_cmd_fwd.y * v23 ) ) + ( norm_cmd_fwd.z * v25 ) )
				+ ( ( ( norm_cmd_fwd.x * v22 ) + ( norm_cmd_fwd.y * v26 ) ) + ( norm_cmd_fwd.z * v28 ) ) )
				+ ( ( ( norm_cmd_fwd.y * v30 ) + ( norm_cmd_fwd.x * v29 ) ) + ( norm_cmd_fwd.z * v27 ) );
			cmd->sidemove = ( ( ( ( norm_cmd_right.x * v24 ) + ( norm_cmd_right.y * v23 ) ) + ( norm_cmd_right.z * v25 ) )
				+ ( ( ( norm_cmd_right.x * v22 ) + ( norm_cmd_right.y * v26 ) ) + ( norm_cmd_right.z * v28 ) ) )
				+ ( ( ( norm_cmd_right.x * v29 ) + ( norm_cmd_right.y * v30 ) ) + ( norm_cmd_right.z * v27 ) );
			cmd->upmove = ( ( ( ( norm_cmd_up.x * v23 ) + ( norm_cmd_up.y * v24 ) ) + ( norm_cmd_up.z * v25 ) )
				+ ( ( ( norm_cmd_up.x * v26 ) + ( norm_cmd_up.y * v22 ) ) + ( norm_cmd_up.z * v28 ) ) )
				+ ( ( ( norm_cmd_up.x * v30 ) + ( norm_cmd_up.y * v29 ) ) + ( norm_cmd_up.z * v27 ) );

			wish_angles = cmd->viewangles;

			if ( g::local->move_type( ) != ( int ) move_type::type_ladder )
				cmd->buttons &= ~( in_forward | in_back | in_moveright | in_moveleft );
		}

#pragma endregion
#pragma region exploit

		bool in_clmove( ) {
			if ( !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) )
				return false;

			if ( !g::local || !g::cmd || !g::local->is_alive( ) )
				return false;

			if ( exploit::shift_amt <= 0 || !exploit::recharge || !g_var.ragebot.exploit || !keyhandler::auto_check( g_var.ragebot.exploit_bind[ 1 ] , g_var.ragebot.exploit_bind[ 0 ] ) )
				return false;

			exploit::shift_amt--;

			if ( exploit::shift_amt < 1 )
			{
				exploit::recharge = false;
				exploit::can_dt_again = true;
				exploit::was_unk = false;
			}
			return true;
		}

		void copy_command( usercmd* cmd , int tickbase_shift )
		{
			static auto cl_forwardspeed = interfaces::cvar->find_var( "cl_forwardspeed" );
			static auto cl_sidespeed = interfaces::cvar->find_var( "cl_sidespeed" );

			if ( !g_var.teleport )
			{
				cmd->forwardmove = 0.0f;
				cmd->sidemove = 0.0f;
			}
			else
			{
				if ( cmd->forwardmove >= 5.0f )
					cmd->forwardmove = cl_forwardspeed->get_float( );
				else if ( cmd->forwardmove <= -5.0f )
					cmd->forwardmove = -cl_forwardspeed->get_float( );

				if ( cmd->sidemove >= 5.0f )
					cmd->sidemove = cl_sidespeed->get_float( );
				else if ( cmd->sidemove <= -5.0f )
					cmd->sidemove = -cl_sidespeed->get_float( );
			}

			auto commands_to_add = 0;

			do
			{
				auto sequence_number = commands_to_add + cmd->command_number;

				auto command = interfaces::input->get_user_cmd( sequence_number );
				auto verified_command = interfaces::input->get_verified_user_cmd( sequence_number );

				memcpy( command , cmd , sizeof( usercmd ) );

				if ( command->tick_count != INT_MAX && interfaces::client_state->m_delta_tick > 0 )
					interfaces::prediction->update( interfaces::client_state->m_delta_tick , true , interfaces::client_state->m_last_command_ack , interfaces::client_state->m_last_outgoing_command + interfaces::client_state->m_choked_commands );

				command->command_number = sequence_number;
				command->hasbeenpredicted = command->tick_count != INT_MAX;

				++interfaces::client_state->m_choked_commands;

				if ( interfaces::client_state->m_net_channel )
				{
					++interfaces::client_state->m_net_channel->choked_packets;
					++interfaces::client_state->m_net_channel->out_sequencenr;
				}

				math::normalize_angle2( command->viewangles );

				memcpy( &verified_command->m_cmd , command , sizeof( usercmd ) );
				verified_command->m_crc = command->GetChecksum( );

				++commands_to_add;
			} while ( commands_to_add != tickbase_shift );

			*( bool* ) ( ( uintptr_t ) interfaces::prediction + 0x24 ) = true;
			*( int* ) ( ( uintptr_t ) interfaces::prediction + 0x1C ) = 0;
		}

		void doubletap( usercmd* cmd )
		{
			if ( !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) ) {
				exploit::can_dt_again = true;
				exploit::shot_tick = -1;
				exploit::shot_tickbase = -1;
				exploit::shift_amt = exploit::tickbase_shift = 0;
				return;
			}

			if ( !g::local || !g::local->is_alive( ) )
				return;
			auto weapon = g::local->weapon( );
			if ( !weapon )
				return;
			auto wpn_data = weapon->weapon_data( );
			if ( !wpn_data )
				return;


			if ( g::cmd->tick_count == exploit::shot_tick + 14 )
			{
				exploit::recharge = true;
				exploit::can_dt_again = false;
				exploit::shift_amt = 16;
			}


			if ( exploit::was_unk && g_var.ragebot.exploit > 0 && keyhandler::auto_check( g_var.ragebot.exploit_bind[ 1 ] , g_var.ragebot.exploit_bind[ 0 ] ) )
			{
				exploit::can_dt_again = true;
				exploit::shift_amt = 16;
				exploit::ready = false;
				exploit::recharge = true;
			}
			static bool was_fd;

			if ( was_fd != keyhandler::auto_check( g_var.antihit.fakeduck_bind[ 1 ] , g_var.antihit.fakeduck_bind[ 0 ] ) )
			{
				if ( !keyhandler::auto_check( g_var.antihit.fakeduck_bind[ 1 ] , g_var.antihit.fakeduck_bind[ 0 ] ) )
					was_fd = keyhandler::auto_check( g_var.antihit.fakeduck_bind[ 1 ] , g_var.antihit.fakeduck_bind[ 0 ] );
				exploit::was_unk = true;
				return;
			}

			static int last_exploit = 0;
			if ( weapon->is_knife( ) || weapon->is_grenade( ) || weapon->is_c4( ) /*|| g_var.ragebot.antiaim.main.duck_key.is_active( ) */ || !g_var.ragebot.exploit || !keyhandler::auto_check( g_var.ragebot.exploit_bind[ 1 ] , g_var.ragebot.exploit_bind[ 0 ] ) )
			{
				exploit::shot_tick = -1;
				exploit::shot_tickbase = -1;
				exploit::shot_cmdnr = -1;
				exploit::ready = false;
				if ( !( weapon->is_knife( ) || weapon->is_grenade( ) || weapon->is_c4( ) /*|| g_var.ragebot.antiaim.main.duck_key.is_active( )*/ ) )
					exploit::was_unk = true;
				return;
			}

			if ( last_exploit != g_var.ragebot.exploit )
			{
				exploit::was_unk = true;
				last_exploit = g_var.ragebot.exploit;
			}

			if ( in_clmove( ) )
			{
				cmd->tick_count = INT_MAX;
				cmd->forwardmove = 0.0f;
				cmd->sidemove = 0.0f;
				cmd->upmove = 0.0f;
				cmd->buttons &= ~in_attack;
				//cmd->buttons &= ~in_attack2;
				return;
			}

			if ( exploit::can_dt_again && exploit::shift_amt < 1 )
				exploit::ready = true;
			else
				exploit::ready = false;

			if ( exploit::can_dt_again && exploit::shift_amt < 1 && cmd->buttons & in_attack ) {
				g::sendpacket = true;
				exploit::shot_cmdnr = cmd->command_number;
				exploit::shot_tick = cmd->tick_count;
				exploit::shot_tickbase = g::local->tick_base( );

				auto next_command_number = cmd->command_number + 1;
				auto user_cmd = interfaces::input->get_user_cmd( next_command_number );

				memcpy( user_cmd , cmd , sizeof( usercmd ) );
				user_cmd->command_number = next_command_number;

				copy_command( user_cmd , 16 );

				exploit::can_dt_again = false;
			}

			if ( !exploit::can_dt_again && cmd->buttons & in_attack && g_var.ragebot.exploit == 1 )
			{
				exploit::can_dt_again = false;
				exploit::shot_tick = cmd->tick_count + 14;
				exploit::did_shot = false;
			}
		}

#pragma endregion
	}
}

