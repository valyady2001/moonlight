#include <features.h>
#include <intrin.h>
#include <mutex>
#include <menu/menu.h>
using namespace features;
#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -MAX_COORD_FLOAT )
using namespace ragebot;
__declspec( naked ) void __fastcall hook::create_move_proxy( void* _this , int , int sequence_number , float input_sample_frametime , bool active )
{
	__asm
	{
		push ebp
		mov  ebp , esp
		push ebx; not sure if we need this
		push esp
		push dword ptr [ active ]
		push dword ptr [ input_sample_frametime ]
		push dword ptr [ sequence_number ]
		call hook::create_move
		pop  ebx
		pop  ebp
		retn 0Ch
	}
}
void auto_pistol( usercmd* cmd )
{
	if ( !cmd )
		return;
	if ( !g::local->is_alive( ) || !g::local )
		return;
	auto wpn = g::local->weapon( );
	if ( !wpn )
		return;

	if ( g::local->is_alive( ) )
		if ( cmd->buttons & in_attack && !wpn->is_sniper( ) && !wpn->is_grenade( ) && !wpn->is_c4( ) && wpn->item_definition_index( ) != item_def_idx::revolver && !wpn->is_knife( ) )
		{
			static bool flipFlop = true;
			if ( flipFlop ) { cmd->buttons |= in_attack; }
			else { cmd->buttons &= ( ~in_attack ); }
			flipFlop = !flipFlop;
		}
}
auto max_accurate_speed = [ & ] ( ) {
	auto wpn = g::local->weapon( );
	if ( !wpn )
		return -1.f;

	return ( wpn->zoom( ) > 0 ? wpn->weapon_data( )->max_speed_alt : wpn->weapon_data( )->max_speed ) / 3.f;
};
void clan_tag( )
{
	auto apply = [ ] ( const char* name ) -> void {
		using Fn = int( __fastcall* )( const char* , const char* );
		static auto fn = reinterpret_cast< Fn >( utilities::find_sig( "engine.dll" , "53 56 57 8B DA 8B F9 FF 15" ) );
		fn( name , name );
	};

	static float last_time;

	if ( int( interfaces::global_vars->curtime ) != last_time )
	{
		std::string name = ( "  moonlight   " );
		apply( ( char* ) name.substr( 0 , int( interfaces::global_vars->curtime ) % name.length( ) ).c_str( ) );
		last_time = int( interfaces::global_vars->curtime + 0.1 );
	}
}

std::mutex create_move_mutex;
void __stdcall hook::create_move( int sequence_number , float input_sample_frametime , bool active , bool& bSendPacket )
{
	create_move_mutex.lock( );
	static auto oCreateMove = create_move_proxy_fn;

	oCreateMove( interfaces::client , 0 , sequence_number , input_sample_frametime , active );

	auto cmd = interfaces::input->get_user_cmd( sequence_number );
	auto verified = interfaces::input->get_verified_user_cmd( sequence_number );

	if ( !cmd || !cmd->command_number ) {
		create_move_mutex.unlock( );
		return;
	}

	if ( interfaces::engine->is_connected( ) && interfaces::engine->is_in_game( ) && g::local ) {
		if ( menu::opened ) {
			if ( cmd->buttons & in_attack )
				cmd->buttons &= ~in_attack;

			if ( cmd->buttons & in_attack2 )
				cmd->buttons &= ~in_attack2;
		}
		cmd->buttons |= in_bullrush;

		g::strafe_angle = cmd->viewangles;
		g::cmd = cmd;
		g::max_speed = max_accurate_speed( );
		visuals::show_impacts( );
		auto_pistol( cmd );
		misc::strafer( cmd );
		misc::bunnyhop( cmd );
		g::unpred_vel = g::local->velocity( );
		misc::fakelag( );
		ragebot::init( );
		g::eye_pos = g::local->eye_pos( );

		prediction_sys::start( cmd );
		{
			ragebot::run( cmd );
			ragebot::handle_misses( );
			if ( g_var.clantag )
				clan_tag( );
			misc::doubletap( cmd );
			misc::fake_duck( cmd );
			antiaim::run( );
			misc::trace( cmd );
			misc::autostop( cmd );

		}prediction_sys::end( );
		misc::small_movement( );
		misc::fix_movement( cmd , g::strafe_angle );
		bSendPacket = g::sendpacket;

		if ( bSendPacket )
			g::sent_angle = cmd->viewangles;
		else
			g::choked_angle = cmd->viewangles;

		animations::update_desync( );
		animations::_realstate.update = true;
	}
	cmd->forwardmove = std::clamp( cmd->forwardmove , -450.0f , 450.0f );
	cmd->sidemove = std::clamp( cmd->sidemove , -450.0f , 450.0f );

	if ( g_var.misc.antiuntrusted )
	{
		cmd->viewangles = math::normalize_angle( cmd->viewangles );
		cmd->viewangles.x = std::clamp( cmd->viewangles.x , -89.0f , 89.0f );
		cmd->viewangles.y = std::clamp( cmd->viewangles.y , -180.0f , 180.0f );
		cmd->viewangles.z = 0.0f;
	}
	verified->m_cmd = *cmd;
	verified->m_crc = cmd->GetChecksum( );
	create_move_mutex.unlock( );
}

std::mutex frame_stage_mutex;
void __fastcall hook::frame_stage_notify( void* ecx , void* edx , int stage ) {
	frame_stage_mutex.lock( );
	static auto mat_postprocess_enable = interfaces::cvar->find_var( "mat_postprocess_enable" );
	static auto r_dynamic = interfaces::cvar->find_var( "r_dynamic" );
	static auto r_dynamiclighting = interfaces::cvar->find_var( "r_dynamiclighting" );

	if ( g_var.esp.removals [ 5 ] )
	{
		mat_postprocess_enable->set_value( 0 );
		r_dynamic->set_value( 0 );
		r_dynamiclighting->set_value( 0 );
	}
	else
	{
		mat_postprocess_enable->set_value( 1 );
		r_dynamic->set_value( 1 );
		r_dynamiclighting->set_value( 1 );
	}

	static auto cl_csm_shadows = interfaces::cvar->find_var( "cl_csm_shadows" );
	static auto cl_csm_world_shadows = interfaces::cvar->find_var( "cl_csm_world_shadows" );
	static auto cl_foot_contact_shadows = interfaces::cvar->find_var( "cl_foot_contact_shadows" );
	static auto cl_csm_static_prop_shadows = interfaces::cvar->find_var( "cl_csm_static_prop_shadows" );
	static auto cl_csm_viewmodel_shadows = interfaces::cvar->find_var( "cl_csm_viewmodel_shadows" );
	static auto cl_csm_rope_shadows = interfaces::cvar->find_var( "cl_csm_rope_shadows" );
	static auto cl_csm_sprite_shadows = interfaces::cvar->find_var( "cl_csm_sprite_shadows" );

	if ( g_var.esp.removals [ 7 ] ) {
		cl_csm_shadows->set_value( 0 );
		cl_csm_world_shadows->set_value( 0 );
		cl_foot_contact_shadows->set_value( 0 );
		cl_csm_static_prop_shadows->set_value( 0 );
		cl_csm_viewmodel_shadows->set_value( 0 );
		cl_csm_rope_shadows->set_value( 0 );
		cl_csm_sprite_shadows->set_value( 0 );
	}
	else {
		cl_csm_shadows->set_value( 1 );
		cl_csm_world_shadows->set_value( 1 );
		cl_foot_contact_shadows->set_value( 1 );
		cl_csm_static_prop_shadows->set_value( 1 );
		cl_csm_viewmodel_shadows->set_value( 1 );
		cl_csm_rope_shadows->set_value( 1 );
		cl_csm_sprite_shadows->set_value( 1 );
	}

	static auto panorama_disable_blur = interfaces::cvar->find_var( ( "@panorama_disable_blur" ) ); panorama_disable_blur->null_callback( );
	static auto panorama_eco_mode = interfaces::cvar->find_var( ( "@panorama_eco_mode" ) ); panorama_eco_mode->null_callback( );

	panorama_disable_blur->set_value( g_var.esp.removals [ 6 ] ? 1 : 0 );
	panorama_eco_mode->set_value( g_var.esp.removals [ 6 ] ? 2 : 1 );


	//static auto cl_interp = interfaces::cvar->find_var( "cl_interp" ); /* self-explanatory TODO: move this in level init */
	//static auto cl_interp_ratio = interfaces::cvar->find_var( "cl_interp_ratio" ); /* self-explanatory */
	static auto cl_extrapolate = interfaces::cvar->find_var( "cl_extrapolate" ); /* does pretty much nothing as far as i know */
	static auto cl_extrapolate_amount = interfaces::cvar->find_var( "cl_extrapolate_amount" ); /* actually disables extrapolation */

	/* set needed values */
	cl_extrapolate->set_value( 0 );
	cl_extrapolate_amount->set_value( 0.f );
	//cl_interp->set_value( interfaces::global_vars->interval_per_tick );
	//cl_interp_ratio->set_value( 2.0f );
	static int night = 0.f;

	if ( !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) ) {
		ragebot::pre_stop = false;
		for ( int i = 0; i <= 64; i++ )
		{
			if ( lagcomp::rec [ i ].size( ) )
				lagcomp::rec [ i ].clear( );
		}
		g::local = nullptr;
		night = 0.f;
		frame_stage_mutex.unlock( );
		return;
	}

	g::local = ( base_entity* ) interfaces::ent_list->get_client_entity( interfaces::engine->get_local_player( ) );
	const auto dur = -1.f;

	if ( interfaces::engine->is_connected( ) && interfaces::engine->is_in_game( ) && g::local ) {

		if ( stage == frame_net_update_postdataupdate_end )
		{
			for ( int i = 1; i <= 64; i++ )
			{
				auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
				if ( !e || !e->is_alive( ) || e == g::local || e->dormant( ) || e->client_class( )->class_id != ( int ) EClassIds::CCSPlayer )
					continue;

				auto map = e->varmap( );
				if ( !map ) return;

				for ( auto i = 0; i < map->m_interpolatedentries; i++ ) {
					map->m_entries [ i ].m_needstointerpolate = false;
				}
			}
		}

		if ( stage == frame_render_start )
		{
			animations::update_local( );
			visuals::remove_flash( );
			visuals::remove_smoke( );

			for ( int i = 1; i <= 64; i++ )
			{
				auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
				if ( !e || !e->is_alive( ) || e == g::local || e->dormant( ) || e->client_class( )->class_id != ( int ) EClassIds::CCSPlayer )
					continue;
				*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t > ( e ) + 0xA30 ) = interfaces::global_vars->framecount;
				//we'll skip occlusion checks now
				*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t > ( e ) + 0xA28 ) = 0; //clear occlusion flags
				*reinterpret_cast< int* > ( reinterpret_cast< uintptr_t > ( e ) + 0xA64 ) = interfaces::global_vars->framecount;
			}

		}
		if ( night != g_var.esp.nightmode )
		{
			visuals::color_modulate_world( );
			night = g_var.esp.nightmode;
		}

	}
	if ( stage == frame_net_update_end )
	{
		for ( int i = 1; i <= 64; i++ )
		{
			auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
			if ( !e || !e->is_alive( ) || e == g::local || e->dormant( ) || e->client_class( )->class_id != ( int ) EClassIds::CCSPlayer )
				continue;
			if ( g_var.ragebot.resolver )
				resolver::resolve( e );

		}
		lagcomp::update_ticks( );
	}

	fsn_fn( ecx , edx , stage );

	if ( stage == frame_net_update_end )
	{




		if ( interfaces::engine->is_connected( ) && interfaces::engine->is_in_game( ) && g::local )
		{

			for ( int i = 1; i <= 64; i++ ) {
				auto e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
				if ( !e || !e->is_alive( ) || e == g::local || e->dormant( ) || e->client_class( )->class_id != ( int ) EClassIds::CCSPlayer )
					continue;



				const auto hViewModel = e->get_viewmodel( );
				if ( hViewModel != 0xFFFFFFFF ) {
					auto pViewModel = ( base_entity* ) interfaces::ent_list->get_by_handle( hViewModel );
					if ( pViewModel ) {
						utilities::UpdateAllViewmodelAddons( pViewModel );
					}
				}
			}

		}
	}

	frame_stage_mutex.unlock( );
}



void __fastcall hook::draw_model_execute( void* ecx , void* edx , void* ctx , void* state , const mdlrender_info_t& info , matrix_t* bone_to_world )
{
	if ( interfaces::model_render->is_forced_mat( ) &&
		!strstr( interfaces::model_info->model_name( info.model ) , "arms" ) &&
		!strstr( interfaces::model_info->model_name( info.model ) , "weapons/v_" ) ) {
		return dme_fn( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );
	}

	if ( strstr( interfaces::model_info->model_name( info.model ) , "shadow" ) )
		return;

	chams::run( dme_fn , ctx , state , info , bone_to_world );
}
void __fastcall hook::lock_cursor( void* ecx , void* edx ) {

	if ( menu::opened ) {
		interfaces::surface->unlock_cursor( );
		return;
	}

	lock_fn( ecx , edx );
}

void __fastcall hook::override_view( void* ecx , void* edx , view_setup* setup ) {

	if ( !g::local || g::local->health( ) <= 0 )
		return;

	if ( interfaces::engine->is_in_game( ) && interfaces::engine->is_connected( ) )
	{
		std::uint32_t weapon_handle = *( std::uint32_t* ) ( uintptr_t( g::local ) + 0x2EF8 );
		auto weapon = ( c_weapon* ) interfaces::ent_list->get_by_handle( weapon_handle );

		if ( weapon && weapon->is_scopeable( ) && g::local->is_scoped( ) ) {
			const float o_setup_fov = setup->fov;

			if ( g_var.esp.removals [ 0 ] )
			{
				if ( weapon->zoom( ) == 2 )
					setup->fov = 50;
				else if ( weapon->zoom( ) == 1 )
					setup->fov = 90;
			}
			else
				setup->fov = o_setup_fov;
		}

		if ( g_var.esp.removals [ 2 ] )
		{
			vec3_t viewPunch = reinterpret_cast< base_entity* >( g::local )->punch_angle( );
			vec3_t aimPunch = reinterpret_cast< base_entity* >( g::local )->aim_punch_angle( );

			setup->angles [ 0 ] -= ( viewPunch [ 0 ] + ( aimPunch [ 0 ] * 2 * 0.4499999f ) );
			setup->angles [ 1 ] -= ( viewPunch [ 1 ] + ( aimPunch [ 1 ] * 2 * 0.4499999f ) );
			setup->angles [ 2 ] -= ( viewPunch [ 2 ] + ( aimPunch [ 2 ] * 2 * 0.4499999f ) );
		}
		misc::thirdperson( );

		if ( keyhandler::auto_check( g_var.antihit.fakeduck_bind [ 1 ] , g_var.antihit.fakeduck_bind [ 0 ] ) )
			setup->origin.z = g::local->abs_origin( ).z + 64.f;
	}

	override_fn( ecx , edx , setup );
}

void __fastcall hook::extra_bone_process( void* ecx , void* edx , int a2 , int a3 , int a4 , int a5 , int a6 , int a7 ) {
	auto e = reinterpret_cast< base_entity* >( ecx );
	if ( !e ) {
		extra_bone_prc_fn( ecx , edx , a2 , a3 , a4 , a5 , a6 , a7 );
		return;
	}

	// get animstate ptr.
	auto state = e->get_animstate( );

	// backup pointer.
	base_entity* backup { nullptr };

	if ( state ) {
		// backup player ptr.
		backup = state->entity;

		// null player ptr, GUWOP gang.
		state->entity = nullptr;
	}

	// call og.
	extra_bone_prc_fn( ecx , edx , a2 , a3 , a4 , a5 , a6 , a7 );

	// restore ptr.
	if ( state && backup )
		state->entity = backup;
}

vec3_t* __fastcall hook::get_eye_angles( void* ecx , void* edx ) {
	if ( ecx != g::local || !g::local )
		return eye_angles_fn( ecx , edx );

	static auto ret_to_thirdperson_pitch = std::uintptr_t( utilities::find_sig( "client.dll" , "8B CE F3 0F 10 00 8B 06 F3 0F 11 45 ? FF 90 ? ? ? ? F3 0F 10 55" ) );
	static auto ret_to_thirdperson_yaw = std::uintptr_t( utilities::find_sig( "client.dll" , "F3 0F 10 55 ? 51 8B 8E" ) );

	if ( std::uintptr_t( _ReturnAddress( ) ) == ret_to_thirdperson_pitch
		|| std::uintptr_t( _ReturnAddress( ) ) == ret_to_thirdperson_yaw )
		return g::cmd ? &g::cmd->viewangles : &g::local->angles( );

	return eye_angles_fn( ecx , edx );
}

void __fastcall hook::build_transformations( void* ecx , void* edx , studiohdr_t* hdr , vec3_t* pos , quaternion* q , const matrix_t& transform , int mask , uint8_t* computed )
{
	auto player = ( base_entity* ) ( ecx );
	if ( !player )
		return transformations_fn( ecx , edx , hdr , pos , q , transform , mask , computed );

	/* backup C_BaseAnimating::m_isJiggleBonesEnabled */
	auto o_jiggle_bones_enabled = player->jiggle_bones_enabled( );

	/* disable jiggle bones ( http://prntscr.com/sj25yo ) to not let valve animate weapons/etc. */
	player->jiggle_bones_enabled( ) = false;

	/* let valve do their things */
	transformations_fn( ecx , edx , hdr , pos , q , transform , mask , computed );

	/* restore jiggle_bones_enabled */
	player->jiggle_bones_enabled( ) = o_jiggle_bones_enabled;
}

bool __fastcall hook::should_skip_animframe( void* ecx , void* edx )
{
	return false;
	/* return o_should_skip_animframe( REG_OUT ); */
}
bool __fastcall hook::setup_bones( void* ecx , void* edx , matrix_t* out , int max_bones , int mask , float curtime ) {

	return setup_bones_fn( ecx , edx , out , max_bones , mask , curtime );
}

void __fastcall hook::standard_blending_rules( void* ecx , void* edx , studiohdr_t* hdr , vec3_t* pos , quaternion* q , float curtime , int mask )
{
	auto pl = ( base_entity* ) ( ecx );
	if ( !pl || !pl->is_player( ) ) {
		return standard_blending_rules_fn( ecx , edx , hdr , pos , q , curtime , mask );
	}

	pl->effects( ) |= ef_nointerp;
	standard_blending_rules_fn( ecx , edx , hdr , pos , q , curtime , mask );
	pl->effects( ) &= ~ef_nointerp;
}

void __fastcall hook::update_client_animations( void* ecx , void* edx )
{
	base_entity* player = ( base_entity* ) ecx;

	if ( !player || !g::local || !g::local->is_alive( ) )
	{
		update_client_animations_fn( ecx , edx );
		return;
	}

	if ( animations::enable_bones )
		update_client_animations_fn( ecx , edx );
}
bool __fastcall hook::is_hltv( void* ecx , void* edx ) {
	static const auto accumulate_layers_call = ( void* ) ( utilities::find_sig( "client.dll" , "84 C0 75 0D F6 87" ) );
	static const auto setupvelocity_call = ( void* ) ( utilities::find_sig( "client.dll" , "84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0" ) );

	if ( !interfaces::engine->is_in_game( ) )
		return hltv_fn( ecx , edx );

	if ( _ReturnAddress( ) == setupvelocity_call && animations::enable_bones )
		return true;

	if ( _ReturnAddress( ) == accumulate_layers_call )
		return true;

	return hltv_fn( ecx , edx );
}

void __fastcall hook::cam_think( vinput* ecx , void* edx ) {
	ecx->m_camera_in_third_person = false;
	ecx->m_cam_command = 1;
	return cam_think_fn( ecx , edx );
}

void __fastcall hook::cam_to_third_person( vinput* ecx , void* edx ) {
	cam_to_third_person_fn( ecx , edx );
	if ( ecx->m_camera_offset.z != g_var.esp.camdist )
		ecx->m_camera_offset.z = g_var.esp.camdist;
}


bool __fastcall hook::sv_cheats_get_bool( void* ecx , void* edx ) {
	if ( !ecx )
		return false;

	static DWORD cam_think = std::uintptr_t( utilities::find_sig( "client.dll" , "85 C0 75 30 38 86" ) );

	static auto cl_extrapolate_ret = ( void* ) utilities::find_sig( "client.dll" , "85 C0 74 22 8B 0D ? ? ? ? 8B 01 8B" );
	static auto hermite_fix_ret = ( void* ) utilities::find_sig( "client.dll" , "0F B6 15 ? ? ? ? 85 C0" );
	static auto cl_interpolate_ret = ( void* ) utilities::find_sig( "client.dll" , "85 C0 BF ? ? ? ? 0F 95 C3" );

	if ( _ReturnAddress( ) == cl_interpolate_ret || _ReturnAddress( ) == hermite_fix_ret || _ReturnAddress( ) == cl_extrapolate_ret )
		return false;

	if ( ( DWORD ) _ReturnAddress( ) == cam_think )
		return true;

	return sv_cheats_get_bool_fn( ecx , nullptr );
}

int __fastcall hook::disable_occulusion( void* bsp , void* edx , vec3_t& mins , vec3_t& maxs , unsigned short* pList , int listMax )
{
	if ( !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) || !g::local )
		return model_occulusion_fn( bsp , edx , mins , maxs , pList , listMax );

	static auto list_leaves_in_box_return = utilities::find_sig( "client.dll" , "89 44 24 14 EB 08 C7 44 24 ? ? ? ? ? 8B 45" );
	if ( _ReturnAddress( ) != ( void* ) list_leaves_in_box_return )
		return model_occulusion_fn( bsp , edx , mins , maxs , pList , listMax );

	struct renderable_info_t {
		void* renderable;
		void* alpha_property;
		int enum_count;
		int render_frame;
		unsigned short first_shadow;
		unsigned short leaf_list;
		short area;
		uint16_t flags;
		uint16_t flags2;
		vec3_t bloated_abs_mins;
		vec3_t bloated_abs_maxs;
		vec3_t abs_mins;
		vec3_t abs_maxs;
		int pad;
	};

	auto info = *( renderable_info_t** ) ( ( uintptr_t ) _AddressOfReturnAddress( ) + 0x14 );
	if ( !info || !info->renderable )
		return model_occulusion_fn( bsp , edx , mins , maxs , pList , listMax );

	/* made it ghetto. true */
	auto get_client_unknown = [ ] ( void* renderable ) {
		typedef void* ( __thiscall* o_fn )( void* );
		return ( *( o_fn** ) renderable ) [ 0 ]( renderable );
	};

	auto get_base_entity = [ ] ( void* c_unk ) {
		typedef base_entity* ( __thiscall* o_fn )( void* );
		return ( *( o_fn** ) c_unk ) [ 7 ]( c_unk );
	};

	auto base_entity = get_base_entity( get_client_unknown( info->renderable ) );
	if ( !base_entity || !base_entity->is_alive( ) || base_entity->client_class( )->class_id != 40 || base_entity->team( ) == g::local->team( ) )
		return model_occulusion_fn( bsp , edx , mins , maxs , pList , listMax );

	*( uint16_t* ) ( uintptr_t( info ) + 0x0016 ) &= ~0x100;
	*( uint16_t* ) ( uintptr_t( info ) + 0x0018 ) |= 0xC0;

	static vec3_t map_min = vec3_t( MIN_COORD_FLOAT , MIN_COORD_FLOAT , MIN_COORD_FLOAT );
	static vec3_t map_max = vec3_t( MAX_COORD_FLOAT , MAX_COORD_FLOAT , MAX_COORD_FLOAT );
	auto count = model_occulusion_fn( bsp , edx , map_min , map_max , pList , listMax );
	return count;
}

void write_cmd( bf_write* buf , usercmd* pin , usercmd* pout )
{
	static DWORD WriteUsercmdF = ( DWORD ) utilities::find_sig( "client.dll" , "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D" );
	__asm
	{
		mov     ecx , buf
		mov     edx , pin
		push	pout
		call    WriteUsercmdF
		add     esp , 4
	}
};

bool __fastcall hook::write_cmd_delta_buf( void* edx , void* ecx , int slot , bf_write* buf , int from , int to , bool new_command )
{
	if ( exploit::tickbase_shift <= 0 || !g::local || !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) )
		return write_cmd_delta_buf_fn( edx , ecx , slot , buf , from , to , new_command );

	if ( from != -1 )
		return true;
	auto backup_cmds = reinterpret_cast< int* >( std::uintptr_t( buf ) - 48 ) , new_cmds = reinterpret_cast< int* >( std::uintptr_t( buf ) - 44 );

	auto new_commands = *new_cmds , next_cmd_number = interfaces::client_state->m_last_outgoing_command + interfaces::client_state->m_choked_commands + 1;
	auto total_new_cmds = std::clamp( exploit::tickbase_shift , 0 , 62 );
	exploit::tickbase_shift = 0;

	*new_cmds = total_new_cmds;
	*backup_cmds = 0;

	for ( to = next_cmd_number - new_commands + 1; to <= next_cmd_number; to++ ) {
		if ( !write_cmd_delta_buf_fn( edx , ecx , slot , buf , from , to , true ) )
			return false;

		from = to;
	}
	auto last_real_cmd = interfaces::input->get_user_cmd( slot , from );
	auto from_cmd = usercmd {};

	if ( last_real_cmd )
		from_cmd = *last_real_cmd;

	auto to_cmd = from_cmd;
	to_cmd.command_number++;
	to_cmd.tick_count += ( ( int ) ( 1.0f / interfaces::global_vars->interval_per_tick ) * 3 );
	for ( auto i = new_commands; i <= total_new_cmds; i++ ) {
		write_cmd( buf , &to_cmd , &from_cmd );
		from_cmd = to_cmd;
		to_cmd.command_number++;
		to_cmd.tick_count++;
	}
	return true;
}

void __cdecl hook::cl_move( float accumulated_extra_samples , bool bFinalTick )
{
	if ( g::local && g::local->is_alive( ) && !keyhandler::auto_check( g_var.antihit.fakeduck_bind [ 1 ] , g_var.antihit.fakeduck_bind [ 0 ] ) )
	{
		if ( misc::in_clmove( ) ) {
			return;
		}
	}
	return cl_move_fn( accumulated_extra_samples , bFinalTick );
	//cl_move_fn( accumulated_extra_samples , bFinalTick );
}


void __fastcall hook::run_command( void* ecx , void* edx , base_entity* pl , usercmd* cmd , void* move_helper )
{
	if ( !pl || !g::local || pl != g::local )
		return run_command_fn( ecx , edx , pl , cmd , move_helper );

	if ( cmd->tick_count > interfaces::global_vars->tickcount + 8 )
	{
		cmd->hasbeenpredicted = true;
		if ( pl->is_alive( ) )
		{
			pl->set_abs_origin( pl->origin( ) );

			if ( interfaces::global_vars->frametime > 0.0f && !interfaces::prediction->m_engine_paused )
				++pl->tick_base( );
		}
		return;
	}

	if ( cmd->command_number == exploit::shot_cmdnr ) {
		pl->tick_base( ) -= 16;
		++pl->tick_base( );
		interfaces::global_vars->curtime = ticks_to_time( pl->tick_base( ) );
	}
	run_command_fn( ecx , edx , pl , cmd , move_helper );

	if ( cmd->command_number == exploit::shot_cmdnr + 1 )
	{
		pl->tick_base( )--;
		interfaces::global_vars->curtime = ticks_to_time( pl->tick_base( ) );
	}
}

_declspec( noinline )void physicssimulate_detour( base_entity* player )
{
	if ( !interfaces::engine->is_connected( ) || !interfaces::engine->is_in_game( ) )
	{
		( ( hook::physics_fn ) hook::physics_simulate_fn )( player );
		return;
	}
	auto simulation_tick = *( int* ) ( ( uintptr_t ) player + 0x2AC );

	if ( player != g::local || !g::local->is_alive( ) || interfaces::global_vars->tickcount == simulation_tick )
	{
		( ( hook::physics_fn ) hook::physics_simulate_fn )( player );
		return;
	}

	g_netdata.apply( );
	( ( hook::physics_fn ) hook::physics_simulate_fn )( player );
	g_netdata.store( );
}

void __fastcall hook::physics_simulate( void* ecx , void* edx )
{
	return physicssimulate_detour( ( base_entity* ) ecx );
}

bool __fastcall hook::in_prediction( void* ecx , void* edx )
{
	static auto maintain_sequence_transitions = ( void* ) ( utilities::find_sig( "client.dll" , "84 C0 74 17 8B 87" ) );
	static auto setup_bones_ptr = ( void* ) ( utilities::find_sig( "client.dll" , "8B 40 ? FF D0 84 C0 74 ? F3 0F 10 05 ? ? ? ? EB ?" ) + 5 );
	if ( _ReturnAddress( ) == maintain_sequence_transitions || _ReturnAddress( ) == setup_bones_ptr )
		return false;

	return in_prediction_fn( ecx , edx );
}
void __fastcall hook::draw_set_color( c_surface* ecx , void* edx , int r , int g , int b , int a )
{
	if ( !g_var.esp.removals [ 1 ] )
		return set_col_fn( ecx , edx , r , g , b , a );

	const auto return_address = uintptr_t( _ReturnAddress( ) );

	static auto return_to_scope_arc = utilities::find_sig( "client.dll" , "6A 00 FF 50 3C 8B 0D ? ? ? ? FF B7 ? ? ? ?" ) + 5;
	static auto return_to_scope_lens = utilities::find_sig( "client.dll" , "FF 50 3C 8B 4C 24 20" ) + 3;

	if ( return_address == return_to_scope_arc
		|| return_address == return_to_scope_lens )
	{
		// We don't want this to draw, so we set the alpha to 0
		return set_col_fn( ecx , edx , r , g , b , 0 );
	}

	set_col_fn( ecx , edx , r , g , b , a );
}

void __fastcall hook::calc_view( void* ecx , void* edx , vec3_t& eye_origin , vec3_t& eye_angles , float& z_near , float& z_far , float& fov )
{
	auto pl = ( base_entity* ) ( ecx );
	if ( !pl || !g::local || pl != g::local )
		return calc_view_fn( ecx , edx , eye_origin , eye_angles , z_near , z_far , fov );

	auto o_use_new_animstate = pl->use_new_animstate( );
	pl->use_new_animstate( ) = false;
	calc_view_fn( ecx , edx , eye_origin , eye_angles , z_near , z_far , fov );
	pl->use_new_animstate( ) = o_use_new_animstate;
}

void __fastcall hook::modify_eye_position( void* this_pointer , void* edx , vec3_t& input_eye_position ) {

	const auto animation_state = reinterpret_cast< animstate* >( this_pointer );

	animation_state->m_smooth_height_valid( ) = false;

	return modify_eye_position_fn( this_pointer , edx , input_eye_position );

}

void __fastcall hook::check_for_sequence_change( void* this_pointer , void* edx , void* hdr , int cur_sequence , bool force_new_sequence , bool interpolate ) {

	return check_for_sequence_change_fn( this_pointer , edx , hdr , cur_sequence , force_new_sequence , false );

}

void __stdcall hook::level_init_pre( const char* map ) {
	//float rate{ 1.f / interfaces::global_vars->interval_per_tick };

	// set rates when joining a server.
	//g_csgo.cl_updaterate->SetValue( rate );
	//g_csgo.cl_cmdrate->SetValue( rate );

	//g_visuals.m_hit_start = g_visuals.m_hit_end = g_visuals.m_hit_duration = 0.f;

	// invoke original method.
	hook::level_init_pre_fn( map );
	//g_hooks.m_client.GetOldMethod< LevelInitPreEntity_t >( CHLClient::LEVELINITPREENTITY )( this , map );
}


void __stdcall hook::level_shutdown( ) {

	g::local = nullptr;

	hook::level_shutdown_fn( );
}

bool __fastcall hook::send_netmsg( net_channel* pNetChan , void* edx , i_net_message& msg , bool bForceReliable , bool bVoice )
{
	if ( msg.get_type( ) == 14 ) // Return and don't send messsage if its FileCRCCheck
		return false;

	if ( msg.get_group( ) == 9 ) // Fix lag when transmitting voice and fakelagging
		bVoice = true;

	return send_netmsg_fn( pNetChan , edx , msg , bForceReliable , bVoice );
}

void __fastcall hook::paint_traverse( void* ecx , void* edx , unsigned int panel , bool ForceRepaint , bool AllowForce ) {
	paint_traverse_fn( ecx , edx , panel , ForceRepaint , AllowForce );
	static unsigned int PanelId = 0;

	if ( !PanelId ) {
		const auto PanelName = interfaces::panel->GetName( panel );
		if ( !strcmp( PanelName , "FocusOverlayPanel" ) ) {
			PanelId = panel;
		}
	}
	else if ( PanelId == panel ) {
		if ( !interfaces::engine->is_in_game( ) || !interfaces::engine->is_connected( ) )
			return;

		if ( !g::local )
			return;

		for ( int i = 1; i <= 64; i++ ) {
			features::visuals::paint_data_valid [ i ] = false;
			base_entity* player = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
			if ( !player ||
				player->dormant( ) ||
				!player->is_alive( ) ||
				player->team( ) == g::local->team( ) ||
				!features::visuals::get_playerbox( player , features::visuals::paint_bbox [ i ] ) )
				continue;

			auto weapon = player->weapon( );
			if ( !weapon )
				continue;

			features::visuals::paint_weapondata [ i ] = weapon->weapon_data( );
			features::visuals::paint_weapon [ i ] = weapon;
			features::visuals::paint_data_valid [ i ] = true;
		}
	}
}

bool __cdecl hook::gloweffectspectator( void* ecx , void* edx , int& GlowStyle , vec3_t& GlowColor , float& AlphaStart , float& Alpha , float& TimeStart , float& TimeTarget , bool& Animate ) {
	if ( !g::local )
		return gloweffectspectator_fn( ecx , edx , GlowStyle , GlowColor , AlphaStart , Alpha , TimeStart , TimeTarget , Animate );

	if ( !g_var.esp.glow )
		return gloweffectspectator_fn( ecx , edx , GlowStyle , GlowColor , AlphaStart , Alpha , TimeStart , TimeTarget , Animate );

	base_entity* player = ( base_entity* ) ecx;
	if ( !player ||
		player->dormant( ) ||
		!player->is_alive( ) ||
		player->team( ) == g::local->team( ) )
		return gloweffectspectator_fn( ecx , edx , GlowStyle , GlowColor , AlphaStart , Alpha , TimeStart , TimeTarget , Animate );

	GlowColor = vec3_t( g_var.esp.glow_clr [ 0 ] ,
		g_var.esp.glow_clr [ 1 ] ,
		g_var.esp.glow_clr [ 2 ] );

	Alpha = g_var.esp.glow_clr [ 3 ];
	GlowStyle = 0;
	return true;
	//return gloweffectspectator_fn( ecx , edx , GlowStyle , GlowColor , AlphaStart , Alpha , TimeStart , TimeTarget , Animate );
}


bool __fastcall hook::fire_event( void* ecx , void* edx ) {
	if ( !g::local || !g::local->is_alive( ) )
		return fire_event_fn( ecx , edx );


	for ( auto event = interfaces::client_state->m_events; event; event = event->next )
	{
		event->flFireDelay = 0.0f;
	}
	return fire_event_fn( ecx , edx );
}