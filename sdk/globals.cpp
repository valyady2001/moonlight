#include "globals.h"
#include <interfaces/interfaces.h>

netdata g_netdata {};

void netdata::store( ) {
	int          tickbase;
	stored_data* data;

	if ( !g::local || !g::local->is_alive( ) ) {
		reset( );
		return;
	}


	// get current record and store data.
	data = &m_data[ interfaces::client_state->m_command_ack % 128 ];

	data->m_tickbase = g::local->tick_base( );
	data->m_punch_angle = g::local->punch_angle( );
	data->m_punch = g::local->aim_punch_angle( );
	data->m_punch_vel = g::local->aim_punch_angle_vel( );
	data->m_view_offset = g::local->view_offset( );
	if ( g::local->weapon( ) )
	{
		data->m_next_primary_attack = g::local->weapon( )->next_primary_attack( );
		data->m_next_attack = g::local->weapon( )->m_flNextAttack( );
	}
}

void netdata::apply( ) {
	stored_data* data;
	vec3_t        punch_delta , punch_vel_delta , punch_angle_delta;
	vec3_t       view_delta;
	float        modifier_delta;

	if ( !g::local || !g::local->is_alive( ) ) {
		reset( );
		return;
	}

	// get current record and validate.
	data = &m_data[ ( interfaces::client_state->m_command_ack - 1 ) % 128 ];

	if ( g::local->tick_base( ) != data->m_tickbase )
		return;

	// get deltas.
	// note - dex;  before, when you stop shooting, punch values would sit around 0.03125 and then goto 0 next update.
	//              with this fix applied, values slowly decay under 0.03125.

	punch_delta = g::local->aim_punch_angle( ) - data->m_punch;
	punch_vel_delta = g::local->aim_punch_angle_vel( ) - data->m_punch_vel;
	view_delta = g::local->view_offset( ) - data->m_view_offset;
	modifier_delta = g::local->velocity_modifier( ) - data->m_velocity_modifier;
	punch_angle_delta = g::local->punch_angle( ) - data->m_punch_angle;

	if ( fabs( punch_delta.x ) < 0.03125f && fabs( punch_delta.y ) < 0.03125f && fabs( punch_delta.z ) < 0.03125f )
		g::local->aim_punch_angle( ) = data->m_punch;

	if ( fabs( punch_vel_delta.x ) < 0.03125f && fabs( punch_vel_delta.y ) < 0.03125f && fabs( punch_vel_delta.z ) < 0.03125f )
		g::local->aim_punch_angle_vel( ) = data->m_punch_vel;

	if ( fabs( punch_angle_delta.x ) < 0.03125f && fabs( punch_angle_delta.y ) < 0.03125f && fabs( punch_angle_delta.z ) < 0.03125f )
		g::local->punch_angle( ) - data->m_punch_angle;

	if ( fabs( view_delta.x ) < 0.03125f && fabs( view_delta.y ) < 0.03125f && fabs( view_delta.z ) < 0.03125f )
		g::local->view_offset( ) = data->m_view_offset;


	if ( g::local->weapon( ) )
	{
		g::local->weapon( )->next_primary_attack( ) = data->m_next_primary_attack;
		g::local->weapon( )->m_flNextAttack( ) = data->m_next_attack;
	}
}

void netdata::reset( ) {
	m_data.fill( stored_data( ) );
}