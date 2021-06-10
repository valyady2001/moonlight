#include <features.h>
#include <sstream>
#pragma comment(lib, "Winmm.lib")
#include "sounds.h"
#include <playsoundapi.h>


namespace features
{
	namespace events
	{
		void c_hooked_events::log_events( )
		{
			//if you don't want log to show up, clear it if not empty already
			if ( !g::local )
			{
				if ( EventList.size( ) > 0 )
					EventList.clear( );
				//and get outta here
				return;
			}

			//doing this because upper left corner in my cheeto is already taken :^)
			//you can remove that, if you'll use hardcoded values
			int screenW , screenH;
			interfaces::engine->get_screen_size( screenW , screenH );

			//getting current "timestamp"
			float CurrentTime = interfaces::global_vars->curtime;

			//Only 9 messages can be shown at the same time
			//Change the variable, if you want more to show up
			//Erasing oldest message here
			if ( EventList.size( ) >= 10 )
				EventList.erase( EventList.begin( ) + 1 );

			//Not sure if I need this check, but let it be
			if ( EventList.size( ) > 0 )
			{
				for ( size_t i = 0; i < EventList.size( ); i++ )
				{
					//erase the message if enough time has passed
					/*if ( EventList[ i ].erase1 < CurrentTime )
					{
						EventList.erase( EventList.begin( ) + i );
						continue;
					}*/
					if ( !EventList[ i ].max_pos )
					{
						EventList[ i ].x1 += 500 * g::animtime;
						EventList[ i ].x2 += 500 * g::animtime;
						EventList[ i ].x1 = std::clamp<int>( EventList[ i ].x1 , -400 , 5 );
						EventList[ i ].x2 = std::clamp<int>( EventList[ i ].x2 , -400 , 5 );
						if ( EventList[ i ].x2 >= 5 )
							EventList[ i ].max_pos = true;
					}

					if ( EventList[ i ].erase1 < CurrentTime )
						EventList[ i ].x1 -= 500 * g::animtime;

					if ( EventList[ i ].erase2 < CurrentTime )
						EventList[ i ].x2 -= 500 * g::animtime;

					if ( EventList[ i ].x2 < -341 )
					{
						EventList[ i ].max_pos = false;
						EventList.erase( EventList.begin( ) + i );
						continue;
					}

					render::filled_rect( vec2_t( EventList[ i ].x2 , 5 + ( 25 * i ) ) , vec2_t( 340 , 20 ) , EventList[ i ].clr );
					render::filled_rect( vec2_t( EventList[ i ].x1 , 5 + ( 25 * i ) ) , vec2_t( 335 , 20 ) , color( 10 , 10 , 10 ) );
					render::text( vec2_t( EventList[ i ].x1 + 5 , ( 20 + ( 25 * i ) ) - 13 ) , EventList[ i ].msg , render::fonts::segoeui , EventList[ i ].clr );
				}
			}
		}

		c_hooked_events events;

		void c_hooked_events::fire_game_event( i_game_event* event ) noexcept {
			auto event_name = event->get_name( );

			if ( !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) )
				return;

			if ( !strcmp( event_name , "player_hurt" ) ) {
				hitmarker_event( event );
			}

			if ( !strcmp( event_name , "item_purchase" ) && g_var.misc.logs[ 0 ] ) {
				purchase_event( event );
			}

			if (/* !strcmp( event_name , "round_start" ) ||*/ !strcmp( event_name , "round_prestart" ) ) {
				for ( int i = 0; i <= 64; i++ )
				{
					ragebot::hitted [ i ] = false;
					ragebot::misses[ i ] = 0;
				}
				buybot_event( event );
			}

			if ( !strcmp( event_name , "bullet_impact" ) ) {
				impact_event( event );
			}
			if ( !strcmp( event_name , "weapon_fire" ) ) {
				weapon_fire( event );
			}

		}
		void c_hooked_events::buybot_event( i_game_event* event ) noexcept {
			if ( !event )
				return;

			auto local_player = reinterpret_cast< base_entity* >( interfaces::ent_list->get_client_entity( interfaces::engine->get_local_player( ) ) );

			if ( !local_player )
				return;

			if ( g_var.misc.buybot.enable )
			{
				switch ( g_var.misc.buybot.primary )
				{
				case 0:break;
				case 1:
					interfaces::engine->client_cmd( "buy ak47" );
					interfaces::engine->client_cmd( "buy m4a1" );
					interfaces::engine->client_cmd( "buy m4a1_silencer" );
					break;
				case 2:
					interfaces::engine->client_cmd( "buy g3sg1" );
					interfaces::engine->client_cmd( "buy scar20" );
					break;
				case 3:
					interfaces::engine->client_cmd( "buy awp" );
					break;
				case 4:
					interfaces::engine->client_cmd( "buy ssg08" );
					break;
				}

				switch ( g_var.misc.buybot.secondary )
				{
				case 0:break;
				case 1:
					interfaces::engine->client_cmd( "buy elite" );
					break;
				case 2:
					interfaces::engine->client_cmd( "buy deagle" );
					interfaces::engine->client_cmd( "buy revolver" );
					break;
				case 3:
					interfaces::engine->client_cmd( "buy fiveseven" );
					interfaces::engine->client_cmd( "buy tec9" );
					break;
				case 4:
					interfaces::engine->client_cmd( "buy p250" );
					break;
				}

				if ( g_var.misc.buybot.utility[ 2 ] )
				{
					interfaces::engine->client_cmd( "buy hegrenade" );
					interfaces::engine->client_cmd( "buy molotov" );
					interfaces::engine->client_cmd( "buy incgrenade" );
					interfaces::engine->client_cmd( "buy smokegrenade" );
				}

				if ( g_var.misc.buybot.utility[ 0 ] )
					interfaces::engine->client_cmd( "buy taser" );

				if ( g_var.misc.buybot.utility[ 1 ] )
					interfaces::engine->client_cmd( "buy vesthelm" );

				if ( g_var.misc.buybot.utility[ 3 ] )
					interfaces::engine->client_cmd( "buy defuser" );
			}
		}
		void c_hooked_events::hitmarker_event( i_game_event* event ) noexcept {
			if ( !event )
				return;

			auto attacker = ( base_entity* ) interfaces::ent_list->get_client_entity( interfaces::engine->get_player_for_user_id( event->get_int( "attacker" ) ) );

			if ( !attacker )
				return;

			auto damage = event->get_int( "dmg_health" );

			auto victim = ( base_entity* ) interfaces::ent_list->get_client_entity( interfaces::engine->get_player_for_user_id( event->get_int( "userid" ) ) );

			if ( !victim ) return;

			auto health = event->get_int( "health" );
			auto hitgroup_str = event->get_int( "hitgroup" );

			player_info_t info;

			event_t csgo_event;
			std::stringstream szMessage;

			if ( attacker == g::local ) {
				interfaces::engine->get_player_info( victim->idx( ) , &info );

				switch ( g_var.misc.sound )
				{
				case 0: break;
				case 1:PlaySoundA( bell , NULL , SND_ASYNC | SND_MEMORY ); break; //bell
				case 2:PlaySoundA( bubble , NULL , SND_ASYNC | SND_MEMORY ); break; //bubble
				case 3:interfaces::surface->play_sound( "buttons\\arena_switch_press_02.wav" ); break; //metal
				}
				ragebot::hitted [ victim->idx( ) ] = true;
				if ( g_var.misc.logs[ 1 ] )
				{
					if ( ragebot::shot_snapshots.size( ) <= 0 )
						return;

					auto& snapshot = ragebot::shot_snapshots.front( );

					csgo_event.clr = color( 255 , 255 , 255 , 255 );

					csgo_event.erase1 = interfaces::global_vars->curtime + 2.f;
					csgo_event.erase2 = interfaces::global_vars->curtime + 2.4f;
					interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[moonlight] " );
					szMessage << "Hurt " << info.name << " in the " << hitgroup_name( hitgroup_str ) << " for " << damage << " damage." << "[ T: " + std::to_string( snapshot.backtrack ) + " BT: " + ( snapshot.backtrack > 0 ? "Y ]" : "N ]" );
					csgo_event.msg = szMessage.str( );
					szMessage << " \n";
					dbg_print( szMessage.str( ).c_str( ) );

					EventList.push_back( csgo_event );
				}

				if ( g_var.esp.kill_effect )
					if ( health <= 0 )
						g::local->get_health_boost_time( ) = interfaces::global_vars->curtime + 1.5f;

				if ( g_var.misc.logs[ 3 ] )
				{
					if ( ragebot::shot_snapshots.size( ) <= 0 )
						return;

					auto& snapshot = ragebot::shot_snapshots.front( );

					if ( !snapshot.bullet_impact )
					{
						snapshot.bullet_impact = true;
						snapshot.first_processed_time = interfaces::global_vars->curtime;
					}
					snapshot.damage = damage;
				}
			}
			else {

				if ( g_var.misc.logs[ 2 ] )
				{
					if ( victim == g::local )
					{
						interfaces::engine->get_player_info( attacker->idx( ) , &info );

						csgo_event.clr = color( 255 , 255 , 255 , 255 );

						csgo_event.erase1 = interfaces::global_vars->curtime + 2.f;
						csgo_event.erase2 = interfaces::global_vars->curtime + 2.4f;
						interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[moonlight] " );
						szMessage << "Harmed by " << info.name << " in the " << hitgroup_name( hitgroup_str ) << " for " << damage << " damage.";
						csgo_event.msg = szMessage.str( );
						szMessage << " \n";
						dbg_print( szMessage.str( ).c_str( ) );

						EventList.push_back( csgo_event );
					}
				}
			}

		}
		void c_hooked_events::impact_event( i_game_event* event ) noexcept {
			if ( !event )
				return;

			auto shooter = reinterpret_cast< base_entity* >( interfaces::ent_list->get_client_entity( interfaces::engine->get_player_for_user_id( event->get_int( "userid" ) ) ) );
			vec3_t bullet_pos( event->get_float( "x" ) , event->get_float( "y" ) , event->get_float( "z" ) );
			if ( !shooter ) return;

			auto local_player = reinterpret_cast< base_entity* >( interfaces::ent_list->get_client_entity( interfaces::engine->get_local_player( ) ) );
			if ( !local_player )
				return;

			bullet_info.push_back( bullet_info_s( shooter->eye_pos( ) , bullet_pos , interfaces::global_vars->curtime , color::blue( 255 ) ) );

			if ( shooter == local_player )
			{
				//if ( g_var.visuals.esp.bullet_tracers )

				if ( g_var.misc.logs[ 3 ] )
				{
					if ( ragebot::shot_snapshots.size( ) <= 0 )
						return;

					auto& snapshot = ragebot::shot_snapshots.front( );

					if ( !snapshot.bullet_impact )
					{
						snapshot.bullet_impact = true;
						snapshot.first_processed_time = interfaces::global_vars->curtime;
						snapshot.impact = bullet_pos;
					}
				}
			}
		}
		void c_hooked_events::purchase_event( i_game_event* event ) noexcept
		{
			if ( !event )
				return;
			if ( !g::local )
				return;
			auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( interfaces::engine->get_player_for_user_id( event->get_int( "userid" ) ) );
			if ( !e || e->team( ) == g::local->team( ) )
				return;

			auto event_weapon = event->get_string( "weapon" );

			if ( event_weapon == "weapon_unknown" )
				return;

			if ( !event_weapon )
				return;

			player_info_t info;
			interfaces::engine->get_player_info( e->idx( ) , &info );

			event_t csgo_event;

			std::stringstream szMessage;

			csgo_event.clr = color( 255 , 255 , 255 , 255 );

			csgo_event.erase1 = interfaces::global_vars->curtime + 2.f;
			csgo_event.erase2 = interfaces::global_vars->curtime + 2.4f;
			interfaces::cvar->console_color_printf( c_color( 0 , 130 , 125 , 255 ) , "[moonlight] " );
			szMessage << info.name << " bought " << event_weapon << ".";
			csgo_event.msg = szMessage.str( );
			szMessage << " \n";
			dbg_print( szMessage.str( ).c_str( ) );

			EventList.push_back( csgo_event );

		}
		void c_hooked_events::weapon_fire( i_game_event* event ) noexcept {
			if ( !g::local )
				return;
			if ( ragebot::shot_snapshots.size( ) <= 0 )
				return;

			auto& snapshot = ragebot::shot_snapshots.front( );

			if ( !snapshot.weapon_fire )
				snapshot.weapon_fire = true;
		}
		int c_hooked_events::get_event_debug_id( void ) noexcept {
			return EVENT_DEBUG_ID_INIT;
		}

		void c_hooked_events::setup( ) noexcept {
			m_i_debug_id = EVENT_DEBUG_ID_INIT;
			interfaces::event_manager->add_listener( this , "player_hurt" , false );
			interfaces::event_manager->add_listener( this , "item_purchase" , false );
			interfaces::event_manager->add_listener( this , "player_footstep" , false );
			interfaces::event_manager->add_listener( this , "player_death" , false );
			interfaces::event_manager->add_listener( this , "bullet_impact" , false );
			interfaces::event_manager->add_listener( this , "round_prestart" , false );
			interfaces::event_manager->add_listener( this , "round_start" , false );
			interfaces::event_manager->add_listener( this , "weapon_fire" , false );
		}

		void c_hooked_events::release( ) noexcept {
			interfaces::event_manager->remove_listener( this );
		}
	}
}