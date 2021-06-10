#include "features.h"

float randnum( float a, float b ) {
	float random = ( ( float ) rand( ) ) / ( float ) RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}
float fov_player( vec3_t ViewOffSet , vec3_t View , base_entity* entity , int hitbox )
{
	const float MaxDegrees = 180.0f;
	vec3_t Angles = View , Origin = ViewOffSet;
	vec3_t Delta( 0 , 0 , 0 ) , Forward( 0 , 0 , 0 );
	vec3_t AimPos = entity->get_bone_pos( hitbox );

	math::angle_2vectors( Angles , &Forward );
	math::vector_substract( AimPos , Origin , Delta );
	math::normalize_2vec( Delta , Delta );

	float DotProduct = Forward.dot( Delta );
	return ( acos( DotProduct ) * ( MaxDegrees / pi_value ) );
}
int get_closest_player( )
{
	int lowest_fov = INT_MAX;
	vec3_t base_angle = interfaces::engine->get_view_angles( );
	int i = -1;
	for ( int index = 1; index <= 64; index++ )
	{
		auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( index );
		if ( !e || !e->is_alive( ) || e->team( ) == g::local->team( ) || e == g::local || e->dormant( ) || e->client_class( )->class_id != ( int ) EClassIds::CCSPlayer )
			continue;
		float fov = fov_player( g::local->eye_pos( ) , base_angle , e , 0 );
		if ( fov < lowest_fov )
		{
			lowest_fov = fov;
			i = index;
		}
	}
	return i;
}
void at_target( )
{
	if ( !g::local || !g::local->is_alive( ) )
		return;
	int lowest_fov = INT_MAX;
	vec3_t base_angle = interfaces::engine->get_view_angles( );
	int idx = get_closest_player( );
	if ( idx != -1 )
	{
		auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( idx );

		if ( e )
			g::cmd->viewangles.y = ( math::calculate_angle( g::local->origin( ) , e->origin( ) ).y + 180 );
	}
	else
		g::cmd->viewangles.y += 180;
}


namespace features
{
	namespace antiaim
	{
		float server_curtime( ) {
			if ( !g::cmd )
				return -1;
			const auto v1 = interfaces::engine->get_net_channel_info( );
			if ( !v1 )
				return 0.f;

			const auto v3 = v1->get_latency( 1 ); //local player
			const auto v4 = v1->get_latency( 0 ); //generic

			return v3 + v4 + ticks_to_time( 1 ) + ticks_to_time( g::cmd->tick_count );
		}

		bool lby_update( ) {
			static auto next_lby_update_time = -1.f;

			auto spawn_time = g::local->spawn_time( );
			auto anim_state = g::local->get_animstate( );
			if ( !anim_state )
				return false;

			const auto current_time = server_curtime( );
			{
				if ( spawn_time != g::local->spawn_time( ) )
				{
					spawn_time = g::local->spawn_time( );
					next_lby_update_time = -1.f;
				}

				if ( anim_state->speed2d > 0.1 || !( g::local->flags( ) & fl_onground ) )
					next_lby_update_time = current_time + 0.22f;
				else if ( next_lby_update_time == -1.f || current_time >= next_lby_update_time )
				{
					next_lby_update_time = current_time + 1.1f;
				}
			}

			if ( anim_state->speed2d > 0.1 )
				return false;

			if ( !( g::local->flags( ) & fl_onground ) )
				return false;

			return next_lby_update_time - current_time <= interfaces::global_vars->interval_per_tick;
		}

		void get_manual_aa_input( )
		{
			if ( !g_var.antihit.manual )
				side = freestanding_aa;
			else
			{
				static size_t lastTime = 0;
				if ( GetAsyncKeyState( g_var.antihit.left_key ) ) { // left
					if ( GetTickCount( ) > lastTime ) {
						if ( side == left_aa )
							side = freestanding_aa;
						else
							side = left_aa;
						lastTime = GetTickCount( ) + 250;
					}
				}
				else if ( GetAsyncKeyState( g_var.antihit.right_key ) ) { // right
					if ( GetTickCount( ) > lastTime ) {
						if ( side == right_aa )
							side = freestanding_aa;
						else
							side = right_aa;
						lastTime = GetTickCount( ) + 250;
					}
				}

			}
		}
		float freestand_real( )
		{
			if (!g::local)
				return 0;
			static float last_real;
			bool no_active = true;
			float bestrotation = 0.f;
			float highestthickness = 0.f;
			vec3_t besthead;

			auto leyepos = g::local->eye_pos( );
			auto headpos = g::local->get_hitbox_pos( 0 );
			auto origin = g::local->abs_origin( );

			auto checkWallThickness = [ & ] ( base_entity* pPlayer, vec3_t newhead ) -> float
			{
				vec3_t endpos1, endpos2;
				vec3_t eyepos = pPlayer->eye_pos( );

				ray_t ray;
				ray.initialize( newhead, eyepos );

				trace_filter_skip_two_entities filter( pPlayer, g::local );

				trace_t trace1, trace2;
				interfaces::trace_ray->trace_ray( ray, mask_shot_brushonly, &filter, &trace1 );

				if (trace1.did_hit( ))
					endpos1 = trace1.end;
				else
					return 0.f;

				ray.initialize( eyepos, newhead );
				interfaces::trace_ray->trace_ray( ray, mask_shot_brushonly, &filter, &trace2 );

				if (trace2.did_hit( ))
					endpos2 = trace2.end;

				float add = newhead.dist_to( eyepos ) - leyepos.dist_to( eyepos ) + 3.f;
				return endpos1.dist_to( endpos2 ) + add / 3;
			};

			int index = get_closest_player( );

			static base_entity* entity;

			if (index != -1)
				entity = ( base_entity* ) interfaces::ent_list->get_client_entity( index );

			float step = ( 2 * pi ) / 18.f;

			float radius = fabs( vec3_t( headpos - origin ).length_2d( ) );

			if (index == -1)
			{
				no_active = true;
			}
			else
			{
				for (float rotation = 0; rotation < ( pi * 2.0 ); rotation += step)
				{
					vec3_t newhead( radius * cos( rotation ) + leyepos.x, radius * sin( rotation ) + leyepos.y, leyepos.z );

					float totalthickness = 0.f;

					no_active = false;

					totalthickness += checkWallThickness( entity, newhead );

					if (totalthickness > highestthickness)
					{
						highestthickness = totalthickness;
						bestrotation = rotation;
						besthead = newhead;
					}
				}
			}

			vec3_t viewangles;
			viewangles = interfaces::engine->get_view_angles( );

			if (no_active)
			{
				return 0;
			}
			else
				return viewangles.y + rad2deg( bestrotation );

			last_real = g::cmd->viewangles.y;
		}
		bool should_change_aa( )
		{
			get_manual_aa_input( );

			if ( g_var.antihit.manual && side != freestanding_aa && g_var.antihit.at_target )
				return false;

			if ( g_var.antihit.manual && side != freestanding_aa && !g_var.antihit.at_target )
				return false;

			if ( g_var.antihit.manual && side != freestanding_aa )
				return false;

			if ( g_var.antihit.manual && side == freestanding_aa && ( g_var.antihit.at_target || g_var.antihit.freestand ) )
				return true;

			if ( g_var.antihit.at_target && !g_var.antihit.manual )
				return true;

			if ( g_var.antihit.manual && side == freestanding_aa )
				return false;


			return false;
		}
		void freestand( )
		{
			if ( !g::local )
				return;
			static float last_real;
			bool no_active = true;
			float bestrotation = 0.f;
			float highestthickness = 0.f;
			vec3_t besthead;

			auto leyepos = g::local->eye_pos( );
			auto headpos = g::local->get_hitbox_pos( 0 );
			auto origin = g::local->abs_origin( );

			auto checkWallThickness = [ & ]( base_entity* pPlayer , vec3_t newhead ) -> float
			{
				vec3_t endpos1 , endpos2;
				vec3_t eyepos = pPlayer->eye_pos( );

				ray_t ray;
				ray.initialize( newhead , eyepos );

				trace_filter_skip_two_entities filter( pPlayer , g::local );

				trace_t trace1 , trace2;
				interfaces::trace_ray->trace_ray( ray , mask_shot_brushonly , &filter , &trace1 );

				if ( trace1.did_hit( ) )
					endpos1 = trace1.end;
				else
					return 0.f;

				ray.initialize( eyepos , newhead );
				interfaces::trace_ray->trace_ray( ray , mask_shot_brushonly , &filter , &trace2 );

				if ( trace2.did_hit( ) )
					endpos2 = trace2.end;

				float add = newhead.dist_to( eyepos ) - leyepos.dist_to( eyepos ) + 3.f;
				return endpos1.dist_to( endpos2 ) + add / 3;
			};

			int index = get_closest_player( );

			static base_entity* entity;

			if ( index != -1 )
				entity = ( base_entity* ) interfaces::ent_list->get_client_entity( index );

			float step = ( 2 * pi ) / 18.f; 

			float radius = fabs( vec3_t( headpos - origin ).length_2d( ) );

			if ( index == -1 )
			{
				no_active = true;
			}
			else
			{
				for ( float rotation = 0; rotation < ( pi * 2.0 ); rotation += step )
				{
					vec3_t newhead( radius * cos( rotation ) + leyepos.x , radius * sin( rotation ) + leyepos.y , leyepos.z );

					float totalthickness = 0.f;

					no_active = false;

					totalthickness += checkWallThickness( entity , newhead );

					if ( totalthickness > highestthickness )
					{
						highestthickness = totalthickness;
						bestrotation = rotation;
						besthead = newhead;
					}
				}
			}



			if ( no_active )
			{
				g::cmd->viewangles.y += 180;
			}
			else
				g::cmd->viewangles.y = rad2deg( bestrotation );

			last_real = g::cmd->viewangles.y;
		}
		void manual( )
		{
			static int angle = 0;

			if ( side == left_aa )
				angle = 90;
			else if ( side == right_aa )
				angle = -90;
			else if ( side == freestanding_aa )
				return;

			vec3_t base_angle = interfaces::engine->get_view_angles( );
			g::cmd->viewangles.y = base_angle.y + angle;
		}
		void opposite()
        {
			

			//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
			if (!g::local || !g::local->is_alive( ))
				return;


			float desync = g::local->desync_amount( );
			float angle = 0.f;

			if (g::local->velocity( ).length( ) < 6 && ( g::local->flags( ) & fl_onground ))
				angle = desync * ( g_var.st_opposite_amt / 100.f );
			else if (g::local->velocity( ).length( ) > 6 && ( g::local->flags( ) & fl_onground ))
				angle = desync * ( g_var.mv_opposite_amt / 100.f );
			else if (!( g::local->flags( ) & fl_onground ))
				angle = desync * ( g_var.air_opposite_amt / 100.f );

			static bool flip = false;
			flip = !flip;

			if (!g::sendpacket)
			{
				if (freestand_real( ) == 0.f) // put your body backwards
					g::cmd->viewangles.y = g::sent_angle.y + flip ? 120 : -120;
				else if (freestand_real( ) < 0.f) // put your body left
					g::cmd->viewangles.y -= angle + flip ? 17 : 120;
				else if (freestand_real( ) > 0.f) // put your body right
					g::cmd->viewangles.y += angle + flip ? 120 : 21;
				else // If anything is not the best just do backwards
					g::cmd->viewangles.y = g::sent_angle.y + flip ? 120 : -120;

				if (lby_update( )) {
					
					if (freestand_real( ) < 0.f)
						g::cmd->viewangles.y += 118;
					if (freestand_real( ) > 0.f)
						g::cmd->viewangles.y -= 118;
					
						
				}
			}
			else
			{
				if (freestand_real( ) > 0.f) // put your body left
					g::cmd->viewangles.y += flip ? 15 : -10;
				else if (freestand_real( ) < 0.f) // put your body right
					g::cmd->viewangles.y -= flip ? 10 : +13;
			}
}
		void inverter(  )
		{

			float desync = g::local->desync_amount( );
			float body_yaw = desync * ( g_var.general_body_yaw / 100.f );
			float inverted = desync * ( g_var.general_inverse_body_yaw / 100.f );
			if (g_var.desync_type == 1)
			{
				if (g::sendpacket)
					keyhandler::auto_check( g_var.antihit.inverter [ 1 ], g_var.antihit.inverter [ 0 ] ) ? g::cmd->viewangles.y -= body_yaw : g::cmd->viewangles.y += inverted;
				else
					keyhandler::auto_check( g_var.antihit.inverter [ 1 ], g_var.antihit.inverter [ 0 ] ) ? g::cmd->viewangles.y += body_yaw : g::cmd->viewangles.y -= inverted;
			}
		}
		void custom_swap( )
		{

			
			//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
			if (!g::local || !g::local->is_alive( ))
				return;

			


			static bool flip = false;
			flip = !flip;




			vec3_t viewang;
			viewang = interfaces::engine->get_view_angles( );


			if (g::sendpacket)
			{
				if (g::local->velocity( ).length( ) > 10)
				{
					if (freestand_real( ) > 0.f) // right
					{
						g::cmd->viewangles.y -= ( flip ? -g_var.fake_jitter /2 : g_var.fake_jitter / 2 );
					}
					else if (freestand_real( )< 0.f)
					{
						g::cmd->viewangles.y += ( flip ? -g_var.fake_jitter /2 : g_var.fake_jitter / 2 );

					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y += ( flip ? -g_var.fake_jitter /2 : g_var.fake_jitter / 2 );
					else
						g::cmd->viewangles.y -= ( flip ? -g_var.fake_jitter /2 : g_var.fake_jitter / 2 );
				}
				else
				{

					if (freestand_real( ) > 0.f) // right
					{
						g::cmd->viewangles.y -= ( flip ? -g_var.fake_jitter / 2 : g_var.fake_jitter / 2 );
					}
					else if (freestand_real( ) < 0.f)
					{
						g::cmd->viewangles.y += ( flip ? -g_var.fake_jitter / 2 : g_var.fake_jitter / 2 );

					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y += ( flip ? -g_var.fake_jitter / 2 : g_var.fake_jitter / 2 );
					else
						g::cmd->viewangles.y -= ( flip ? -g_var.fake_jitter / 2 : g_var.fake_jitter / 2 );
				}
			}
			else
			{
				if (g::local->velocity( ).length( ) > 10)
				{
					if (freestand_real( ) < 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? -g_var.real_jitter /2 : g_var.real_jitter / 2 );
					}
					else if (freestand_real( ) > 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ?- g_var.real_jitter /2 : g_var.real_jitter / 2 );
					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? -g_var.real_jitter /2 : g_var.real_jitter / 2 );
					else
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ? -g_var.real_jitter /2 : g_var.real_jitter / 2 );


				}
				else
				{

					if (freestand_real( ) < 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? -g_var.real_jitter / 2 : g_var.real_jitter / 2 );
					}
					else if (freestand_real( ) > 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ? -g_var.real_jitter / 2 : g_var.real_jitter / 2 );
					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? -g_var.real_jitter / 2 : g_var.real_jitter / 2 );
					else
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ? -g_var.real_jitter / 2 : g_var.real_jitter / 2 );
				}
			}

			if (!g::sendpacket)
			{
				if (lby_update) {
					
					if (freestand_real( ) < 0.f)
						g::cmd->viewangles.y += 118;
					if (freestand_real( ) > 0.f)
						g::cmd->viewangles.y -= 118;
				
				}
			}
		}
		void auto_swap( )
		{

		
			//C_BasePlayer* pLocal = (C_BasePlayer*)pEntityList->GetClientEntity(pEngine->GetLocalPlayer());
			if (!g::local || !g::local->is_alive( ))
				return;



			static bool flip = false;
			flip = !flip;




			vec3_t viewang;
			viewang = interfaces::engine->get_view_angles(  );


			if (g::sendpacket)
			{
				if (g::local->velocity( ).length( ) > 10)
				{
					if (freestand_real( ) > 0.f) // right
					{
						g::cmd->viewangles.y -= ( flip ? 12 : 6 );
					}
					else if (freestand_real( ) < 0.f)
					{
						g::cmd->viewangles.y += ( flip ? 24 : 12 );

					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y += ( flip ? randnum( 12, 7 ) : randnum( -4, 16 ) );
					else
						g::cmd->viewangles.y -= ( flip ? randnum( 16, 10 ) : randnum( -8, 19 ) );
				}
				else
				{

					if (freestand_real( ) > 0.f)
					{
						g::cmd->viewangles.y -= ( flip ? 22 : +12 );
					}
					else if (freestand_real( ) < 0.f)
					{
						g::cmd->viewangles.y += ( flip ? -10 : 21 );

					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y += ( flip ? randnum( 4, 16 ) : randnum( -7, 15 ) );
					else
						g::cmd->viewangles.y -= ( flip ? randnum( 4, 16 ) : randnum( -7, 15 ) );
				}
			}
			else
			{
				if (g::local->velocity( ).length( ) > 10)
				{
					if (freestand_real( ) < 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? 98 : randnum( -35, 56 ) );
					}
					else if (freestand_real( ) > 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ? 120 : randnum( -35, 56 ) );
					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? randnum( 18, 35 ) : randnum( -35, 56 ) );
					else
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ? randnum( 18, 35 ) : randnum( -35, 56 ) );


				}
				else
				{

					if (freestand_real( ) < 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? 120 : randnum( -20, 59 ) );

					}
					else if (freestand_real( ) > 0.f)
					{
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ? 120 : randnum( -20, 59 ) );
					}
					else if (freestand_real( ) == 0.f)
						g::cmd->viewangles.y = g::sent_angle.y + ( flip ? randnum( 30, 45 ) : randnum( -20, 59 ) );
					else
						g::cmd->viewangles.y = g::sent_angle.y - ( flip ? randnum( 38, 41 ) : randnum( -20, 59 ) );
				}
			}

			if (!g::sendpacket)
			{
				if (lby_update) {
					if (freestand_real( ) < 0.f)
						g::cmd->viewangles.y += 118;
					if (freestand_real( ) > 0.f)
						g::cmd->viewangles.y -= 118;
				}
			}
		}
		void run( )
		{
			if ( !g_var.antihit.enable )
				return;

			if ( !g::local || !g::local->is_alive( ) ) return;
			auto weapon = g::local->weapon( );
			if ( !weapon ) return;
			if ( g::local->move_type( ) == ( int ) move_type::type_ladder ) return;
			if ( g::local->move_type( ) == ( int ) move_type::type_noclip ) return;
			if ( g::cmd->buttons & in_attack && !weapon->is_grenade( ) )
				return;

			if ( g::cmd->buttons & in_use )
				return;

			if ( weapon->is_grenade( ) )
				if ( weapon->throw_time( ) > 0 )
					return;

			switch ( g_var.antihit.pitch )
			{
			case 0: g::cmd->viewangles.x = 89; break;
			case 1: g::cmd->viewangles.x = -89; break;
			case 2: g::cmd->viewangles.x = 0; break;
			case 3: g::cmd->viewangles.x = 991; break;
			case 4: g::cmd->viewangles.x = -991; break;
			}

			auto run_others = [ ]( )
			{
				g::cmd->viewangles.y += 180;
			};

			auto run_freestanding = [ ]( )
			{
				if ( g_var.antihit.at_target )
					at_target( );
				else
					freestand( );
			};

			should_change_aa( ) ? run_freestanding( ) : g_var.antihit.manual && side != freestanding_aa ? manual( ) : run_others( );

			switch (g_var.desync_type) {
			case 0 :
				break;
			case 1:inverter( );
				   break;
			case 2:auto_swap( );
				break;
			case 3:
				opposite( );
				break;
			case 4: custom_swap( );
				break;
			/*case 5:
				break;*/
			}
		}
	}
}