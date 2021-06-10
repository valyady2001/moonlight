#pragma once
#include "classes/base_entity.h"
#include "classes/usercmd.h"
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <chrono>

#define tick_interval			( interfaces::global_vars->interval_per_tick )
#define time_to_ticks( dt )		( floorf(( 0.5f + (float)(dt) / tick_interval ) ) )
#define ticks_to_time( t )		( tick_interval *( t ) )

namespace g
{
	inline base_entity* local;
	inline usercmd* cmd;
	inline vec3_t sent_angle;
	inline vec3_t eye_pos;
	inline vec3_t choked_angle;
	inline vec3_t strafe_angle;
	inline bool sendpacket = true;
	inline vec3_t unpred_vel;
	inline int _stage;
	inline float max_speed;
	inline bool aimbot_shot[ 64 ];
	inline auto tp1 = std::chrono::system_clock::now( );
	inline auto tp2 = std::chrono::system_clock::now( );
	inline float animtime;
}



namespace exploit
{
	inline int shot_tickbase;
	inline int shot_cmdnr;
	inline int shot_tick;
	inline int tickbase_shift;
	inline int shift_rate;
	inline bool can_dt_again;
	inline int shift_amt;
	inline bool allow_lag;
	inline bool ready;
	inline usercmd shot_cmd;
	inline bool recharge;
	inline bool was_unk;
	inline bool did_shot;
}

namespace keyhandler {

	inline bool is_key_down( int key ) {
		return HIWORD( GetKeyState( key ) );
	}

	inline bool is_key_up( int key ) {
		return !HIWORD( GetKeyState( key ) );
	}

	inline bool is_key_pressed( int key ) {
		return false;
	}

	inline bool auto_check( int key_style , int key ) {
		switch ( key_style ) {
		case 0:
			return true;
		case 1:
			return is_key_down( key );
		case 2:
			return LOWORD( GetKeyState( key ) );
		case 3:
			return is_key_up( key );
		default:
			return true;
		}
	}

	static bool keys[ 256 ] = { false };
	static bool pressed[ 256 ] = { false };
	static bool toggles[ 256 ] = { false };
};

class netdata {
private:
	class stored_data {
	public:
		int    m_tickbase;
		vec3_t  m_punch_angle;
		vec3_t  m_punch;
		vec3_t  m_punch_vel;
		vec3_t m_view_offset;
		float  m_velocity_modifier;
		float m_next_primary_attack;
		float m_next_attack;

	public:
		__forceinline stored_data( ) : m_tickbase{ } , m_next_attack{ } , m_punch_angle{} , m_punch{ } , m_punch_vel{ } , m_view_offset{ } , m_velocity_modifier{ } , m_next_primary_attack{ } {};
	};

	std::array< stored_data , 128 > m_data;

public:
	void store( );
	void apply( );
	void reset( );
};

extern netdata g_netdata;

