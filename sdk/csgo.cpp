#include "csgo.h"
#include <math/math.h>
#include <classes/base_entity.h>
#include <interfaces/interfaces.h>
#include "globals.h"
#define MAX_CMD_BUFFER   4000
namespace csgo
{
	void __cdecl rebuilt_cl_move( float accumulated_extra_samples , bool bFinalTick )
	{
		if ( !g::local )
			return rb_clmove( accumulated_extra_samples , bFinalTick );
		if ( GetAsyncKeyState( 'J' ) )
			return;
		static int shift;
		if ( g::cmd )
			if ( g::cmd->buttons & in_attack && !shift )
				shift = 15;
		shift = std::clamp<int>( shift , 0 , 16 );
		auto netchan = interfaces::client_state->m_net_channel;
		const auto o_choke = interfaces::client_state->m_choked_commands;
		const auto o_packet = netchan->choked_packets;
		const auto o_seq = netchan->choked_packets;
		if ( shift && netchan )
		{
			int nextcommandnr = interfaces::client_state->m_last_outgoing_command + interfaces::client_state->m_choked_commands + 1;
			for ( int i = 0; i < shift; i++ )
			{
				rb_clmove( accumulated_extra_samples++ , false );
				//rb_clmove( accumulated_extra_samples , bFinalTick );
				if ( g::cmd )
					g::cmd->tick_count = INT_MAX;
				rb_clmove( 0 , bFinalTick );

				interfaces::client_state->m_choked_commands++;

				netchan->choked_packets++;
				netchan->out_sequencenr++;

				--shift;
			}
			rb_clmove( 0 , true );
			/*interfaces::client_state->m_choked_commands = o_choke;

			netchan->choked_packets = o_packet;
			netchan->out_sequencenr = o_seq;*/

			interfaces::prediction->m_previous_startframe = -1;
			interfaces::prediction->m_commands_predicted = 0;
		}

		rb_clmove( accumulated_extra_samples , bFinalTick );
	}

	void SetupLean( base_entity* e )
	{
		if ( !e || !e->is_alive( ) || !e->is_player( ) )
			return;
		auto animstate = e->get_animstate( );
		if ( !animstate )
			return;

		using layer_seq_fn = float( __thiscall* )( void* , const char* );
		static auto lookup_sequence = ( layer_seq_fn ) utilities::find_sig( "client.dll" , "55 8B EC 56 8B F1 83 BE ? ? ? ? ? 75 14 8B 46 04 8D 4E 04 FF 50 20 85 C0" );

		float flInterval = interfaces::global_vars->curtime - *( float* ) ( ( uintptr_t( animstate ) ) + 344 );
		if ( flInterval > 0.025f )
		{
			if ( flInterval >= 0.1 )
				flInterval = 0.1;

			*( float* ) ( ( uintptr_t( animstate ) ) + 0x158 ) = interfaces::global_vars->curtime;
			auto v7 = ( vec3_t ) ( *( vec3_t* ) ( ( uintptr_t( e ) ) + 280 ) - *( vec3_t* ) ( ( uintptr_t( animstate ) ) + 352 ) ) * ( float ) ( 1.0 / flInterval );

			v7.z = 0;

			*( float* ) ( ( uintptr_t( animstate ) ) + 360 ) = ( float ) ( *( float* ) ( ( uintptr_t( e ) ) + 276 ) - *( float* ) ( ( uintptr_t( animstate ) ) + 348 ) )
				* ( float ) ( 1.0 / flInterval );

			*( vec3_t* ) ( ( uintptr_t( animstate ) ) + 364 ) = v7;

			*( float* ) ( ( uintptr_t( animstate ) ) + 352 ) = *( float* ) ( ( uintptr_t( e ) ) + 280 );

		}
		auto v9 = *( float* ) ( ( uintptr_t( animstate ) ) + 372 );//aici valve suge pula
		vec3_t v10 = math::vector_approach( *( vec3_t* ) ( ( uintptr_t( animstate ) ) + 0x168 ) , *( vec3_t* ) ( ( uintptr_t( animstate ) ) + 0x17C ) , animstate->last_clientside_anim_update_time_delta * 800.f );
		v9 = v10.x;

		( *( vec3_t* ) ( ( uintptr_t( animstate ) ) + 376 ) )[ 1 ] = v10[ 1 ];
		( *( vec3_t* ) ( ( uintptr_t( animstate ) ) + 380 ) )[ 2 ] = v10[ 2 ];

		vec3_t temp;
		vec3_t forward = vec3_t( 0 , 0 , 1 );
		math::vector_angles3( *( vec3_t* ) ( ( uintptr_t( animstate ) ) + 0x17C ) , forward , temp );

		auto v12 = ( float ) ( sqrt(
			( float ) ( ( float ) ( v9 * v9 ) + ( float ) ( *( float* ) ( ( uintptr_t( animstate ) ) + 376 ) * *( float* ) ( ( uintptr_t( animstate ) ) + 376 ) ) )
			+ ( float ) ( *( float* ) ( ( uintptr_t( animstate ) ) + 380 ) * *( float* ) ( ( uintptr_t( animstate ) ) + 380 ) ) )
			* 0.0038461538 )
			* *( float* ) ( ( uintptr_t( animstate ) ) + 244 );

		auto v13 = 0.f;
		if ( v12 >= 0.0 )
			v13 = fminf( v12 , 1.0 );
		else
			v13 = 0.0;

		*( float* ) ( ( uintptr_t( animstate ) ) + 384 ) = ( float ) ( 1.0 - *( float* ) ( ( uintptr_t( animstate ) ) + 300 ) ) * v13;

		animstate->lean_yaw_pose.set_value( e , math::normalize_yaw( animstate->abs_yaw - temp.y ) );
		if ( e->anim_layer( 12 )->sequence <= 0 )
		{
			e->anim_layer( 12 )->sequence = lookup_sequence( e , "lean" );
		}
		e->anim_layer( 12 )->weight = *( float* ) ( ( uintptr_t( animstate ) ) + 384 );
	}
}