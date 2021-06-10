#pragma once
#include <sdk/defines.h>
#include <sdk/utils/recv.h>


enum clientframestage_t : int
{
	frame_undefined = -1 ,
	frame_start ,
	frame_net_update_start ,
	frame_net_update_postdataupdate_start ,
	frame_net_update_postdataupdate_end ,
	frame_net_update_end ,
	frame_render_start ,
	frame_render_end
};

using create_client_class_t = void* ( * )( int ent_num , int serial_num );
using create_event_t = void* ( * )( );

struct client_class_t {
public:
	create_client_class_t create_fn;
	create_event_t create_event_fn;
	char* network_name;
	recv_table* recv_table;
	client_class_t* next;
	int class_id;
};

struct view_setup {
	__int32   x;                  //0x0000 
	__int32   x_old;              //0x0004 
	__int32   y;                  //0x0008 
	__int32   y_old;              //0x000C 
	__int32   width;              //0x0010 
	__int32   width_old;          //0x0014 
	__int32   height;             //0x0018 
	__int32   height_old;         //0x001C 
	char      pad_0x0020[ 0x90 ];   //0x0020
	float     fov;                //0x00B0 
	float     viewmodel_fov;      //0x00B4 
	vec3_t    origin;             //0x00B8 
	vec3_t    angles;             //0x00C4 
	char      pad_0x00D0[ 0x7C ];   //0x00D0
};//Size=0x014
class bf_write;
class c_client {
public:
	client_class_t* get_all_classes( ) {
		using get_all_classes_fn = client_class_t * ( __thiscall* )( void* );
		return vfunc< get_all_classes_fn >( this , 8 )( this );
	}
	bool dispach_user_message( int message_type , int arg , int arg1 , void* data )
	{
		using DispatchUserMessage_t = bool* ( __thiscall* )( void* , int , int , int , void* );
		return vfunc<DispatchUserMessage_t>( this , 38 )( this , message_type , arg , arg1 , data );
	}

	bool WriteUsercmdDeltaToBuffer( int nSlot , bf_write* buf , int from , int to , bool isnewcommand )
	{
		using o_fn = bool* ( __thiscall* )( void* , int , bf_write* , int , int , bool );
		return vfunc<o_fn>( this , 23 )( this , nSlot , buf , from , to , isnewcommand );
	}

	void CreateMove( int cmdnr , float input_sample_frametime , bool paused )
	{
		using o_fn = void( __thiscall* )( void* , int , float , bool );
		return ( *( o_fn** ) this )[ 22 ]( this , cmdnr , input_sample_frametime , paused );
	}

};