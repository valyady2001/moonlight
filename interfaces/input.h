#pragma once
#include <sdk/math/vec3.h>
#include <sdk/classes/usercmd.h>

#define MULTIPLAYER_BACKUP 150

class c_input
{
public:
	void* pvftable{};
	bool trackir_available{};
	bool mouse_initialized{};
	bool mouse_active{};
	bool joystick_advanced_init{};
	char pad_0x08[ 0x2C ]{};
	void* keys{};
	char pad_0x38[ 0x64 ]{};
	int pad_0x41{};
	int pad_0x42{};
	int pad_0x43{};
	int pad_0x44{};
	bool camera_intercepting_mouse{};
	bool camera_in_thirdperson{};
	bool camera_moving_with_mouse{};
	vec3_t camera_offset;
	bool camera_distance_move{};
	int camera_old_x{};
	int camera_old_y{};
	int camera_x{};
	int camera_y{};
	bool camera_is_orthographic{};
	vec3_t previous_view_angles;
	vec3_t previous_view_angles_tilt;
	float last_forward_move{};
	int clear_input_state{};
	usercmd* commands{};
	verified_usercmd* verified_commands{};
	usercmd* get_user_cmd(int sequence_number) {
		using o_getusercmd = usercmd * (__thiscall*)(void*, int, int);
		return &commands[ sequence_number % 150 ];
		//return (*(o_getusercmd**)this)[8](this, 0, sequence_number);
	}
	usercmd* get_user_cmd2( int sequence_number ) {
		using o_getusercmd = usercmd * ( __thiscall* )( void* , int , int );
		//return &commands[ sequence_number % 150 ];
		return (*(o_getusercmd**)this)[8](this, 0, sequence_number);
	}
	usercmd* get_user_cmd( int slot , int sequence_number ) {
		using o_getusercmd = usercmd * ( __thiscall* )( void* , int , int );
		return ( *( o_getusercmd * * ) this )[ 8 ]( this , slot , sequence_number );
	}
	verified_usercmd* get_verified_user_cmd(int sequence_number) {
		auto verifiedCommands = *(verified_usercmd**)(reinterpret_cast<uint32_t>(this) + 0xF8);
		return &verifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
	}
};


class vinput {
public:

	// public members

	char _pad0[ 0xad ];
	bool m_camera_in_third_person;
	char _pad1[ 0x2 ];
	vec3_t m_camera_offset;
	char _pad2[ 0x44 ];
	int m_camera_third_data;
	int m_cam_command;

};