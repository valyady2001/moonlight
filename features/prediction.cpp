#include "features.h"

#define COMMENTED_PREDICTION

void update_button_state( base_entity* pl , int buttons ) {
	/* https://github.com/perilouswithadollarsign/cstrike15_src/blob/29e4c1fda9698d5cebcdaf1a0de4b829fa149bf8/game/shared/baseplayer_shared.cpp#L961 */

	int buttons_changed = buttons ^ *( int* ) ( std::uintptr_t( pl ) + 0x31F8 );

	*( int* ) ( std::uintptr_t( pl ) + 0x31EC ) = *( int* ) ( std::uintptr_t( pl ) + 0x31F8 );
	*( int* ) ( std::uintptr_t( pl ) + 0x31F8 ) = buttons;
	/* m_afButtonPressed */ *( int* ) ( std::uintptr_t( pl ) + 0x31F0 ) = buttons & buttons_changed;
	/* m_afButtonReleased */ *( int* ) ( std::uintptr_t( pl ) + 0x31F4 ) = buttons_changed & ~buttons;
}
float frametime;
float curtime;
int flags;
vec3_t velocity;
player_move_data* movedata;
uintptr_t prediction_player;
uintptr_t prediction_seed;
bool started;
namespace features
{
	namespace prediction_sys
	{
		void start( usercmd* cmd )
		{
			started = false;
			if ( !interfaces::engine->is_in_game( ) || !cmd || !g::local || !g::local->is_alive( ) ) {
				started = false;
				return;
			}
			interfaces::prediction->update(
				interfaces::client_state->m_delta_tick ,
				interfaces::client_state->m_delta_tick > 0 ,
				interfaces::client_state->m_last_command_ack ,
				interfaces::client_state->m_last_outgoing_command + interfaces::client_state->m_choked_commands );

			if ( !movedata )
				movedata = ( player_move_data* ) std::malloc( 182 );

			if ( !prediction_player || !prediction_seed ) {
				prediction_seed = *reinterpret_cast< std::uintptr_t* >( std::uintptr_t( utilities::find_sig( "client.dll" , "A3 ? ? ? ? 66 0F 6E 86" ) ) + 1 );

				prediction_player = *reinterpret_cast< std::uintptr_t* >( std::uintptr_t( utilities::find_sig( "client.dll" , "89 35 ? ? ? ? F3 0F 10 48 20" ) ) + 2 );
			}
			*reinterpret_cast< int* >( prediction_seed ) = cmd ? cmd->random_seed : -1;
			*reinterpret_cast< int* >( prediction_player ) = reinterpret_cast< int >( g::local );
			*reinterpret_cast< usercmd** >( std::uintptr_t( g::local ) + 0x3338 ) = cmd; /* 0x3334 */
			*reinterpret_cast< usercmd** >( std::uintptr_t( g::local ) + 0x3288 ) = cmd;

			flags = g::local->flags( );
			velocity = g::local->velocity( );

			curtime = interfaces::global_vars->curtime;
			frametime = interfaces::global_vars->frametime;

			const int old_tickbase = g::local->tick_base( );
			const bool old_in_prediction = interfaces::prediction->m_in_prediction;
			const bool old_first_prediction = interfaces::prediction->m_first_time_predicted;

			interfaces::global_vars->curtime = g::local->tick_base( ) * interfaces::global_vars->interval_per_tick;
			interfaces::global_vars->frametime = interfaces::prediction->m_engine_paused ? 0 : interfaces::global_vars->interval_per_tick;

			interfaces::prediction->m_first_time_predicted = false;
			interfaces::prediction->m_in_prediction = true;

			if ( cmd->impulse )
				*reinterpret_cast< std::uint32_t* >( std::uintptr_t( g::local ) + 0x31FC ) = cmd->impulse;

			cmd->buttons |= *( std::uint32_t* ) ( std::uintptr_t( g::local ) + 0x3334 ); /* m_afButtonForced */
			cmd->buttons &= ~*( std::uint32_t* ) ( std::uintptr_t( g::local ) + 0x3330 ); /* m_afButtonDisabled */

			update_button_state( g::local , cmd->buttons );

			interfaces::prediction->check_moving_ground( g::local , interfaces::global_vars->frametime );

			interfaces::game_movement->start_track_prediction_errors( g::local );

			interfaces::move_helper->set_host( g::local );
			interfaces::prediction->setup_move( g::local , cmd , interfaces::move_helper , movedata );
			interfaces::game_movement->process_movement( g::local , movedata );
			interfaces::prediction->finish_move( g::local , cmd , movedata );
			interfaces::move_helper->process_impacts( );

			g::local->tick_base( ) = old_tickbase;

			interfaces::prediction->m_first_time_predicted = old_first_prediction;
			interfaces::prediction->m_in_prediction = old_in_prediction;

			started = true;
		}


		void end( )
		{
			if ( !g::local || !interfaces::move_helper )
				return;
			interfaces::game_movement->finish_track_prediction_errors( g::local );
			*reinterpret_cast< std::uint32_t* >( reinterpret_cast< std::uintptr_t >( g::local ) + 0x3338 /* 0x3334 */ ) = 0;

			if ( prediction_player && prediction_seed )
			{
				*reinterpret_cast< int* >( prediction_seed ) = 0xffffffff;
				*reinterpret_cast< int* >( prediction_player ) = 0;
			}
			interfaces::move_helper->set_host( nullptr );
			interfaces::game_movement->reset( );
			if ( !interfaces::prediction->m_engine_paused && interfaces::global_vars->frametime > 0 ) {
				g::local->tick_base( )++;
			}
			interfaces::global_vars->curtime = curtime;
			interfaces::global_vars->frametime = frametime;

		}
	}
}
