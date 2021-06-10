#include <hook.h>
#include <intrin.h>
#include "../../render/render.h"
#include "../../vars.h"
#include <features.h>
#include "../../menu/menu.h"

bool should_reset = false;
long __fastcall hook::end_scene( void* edx , void* ecx , IDirect3DDevice9* device ) {
	static bool initialized = false;

	HRESULT cooperative_status = device->TestCooperativeLevel( );
	if ( cooperative_status == D3DERR_DEVICELOST ) {
		render::reset( );
		should_reset = true;
	}
	else if ( cooperative_status == D3D_OK && should_reset ) {
		render::initialize( device );
		should_reset = false;
	}
	if ( !initialized ) {
		menu::apply_fonts( );
		menu::set_theme( );
		initialized = true;
	}


	const auto ret_add = std::uintptr_t( _ReturnAddress( ) );
	static std::uintptr_t normal_address = ret_add;
	if ( ret_add != normal_address )
		return endscene_fn( edx , ecx , device );

	g::tp2 = std::chrono::system_clock::now( );
	std::chrono::duration<float> elapsed = g::tp2 - g::tp1;
	g::tp1 = g::tp2;
	g::animtime = elapsed.count( );

	//interfaces::cvar->find_var( "r_modelAmbientMin" )->set_value( 1.25f );
	interfaces::cvar->find_var( "mat_fullbright" )->set_value( g_var.esp.fullbright );

	static auto viewmodel_fov = interfaces::cvar->find_var( "viewmodel_fov" );
	static auto viewmodel_fov_x = interfaces::cvar->find_var( "viewmodel_offset_x" );
	static auto viewmodel_fov_y = interfaces::cvar->find_var( "viewmodel_offset_y" );
	static auto viewmodel_fov_z = interfaces::cvar->find_var( "viewmodel_offset_z" );
	static auto sv_impacts = interfaces::cvar->find_var( "sv_showimpacts" );
	static auto camdist = interfaces::cvar->find_var( "cam_idealdist" );
	static auto cam_lag = interfaces::cvar->find_var( "cam_ideallag" );

	viewmodel_fov->null_callback( );
	sv_impacts->null_callback( );
	viewmodel_fov_x->null_callback( );
	viewmodel_fov_y->null_callback( );
	viewmodel_fov_z->null_callback( );
	camdist->null_callback( );
	cam_lag->null_callback( );
	static bool do_once = false;

	if ( sv_impacts->get_int( ) != g_var.esp.sv_impacts )
		sv_impacts->set_value( g_var.esp.sv_impacts );

	if ( cam_lag->get_float( ) != 0 )
		cam_lag->set_value( 0 );

	if ( !do_once )
	{
		g_var.esp.viewfov = viewmodel_fov->get_int( );
		g_var.esp.view_x = viewmodel_fov_x->get_int( );
		g_var.esp.view_y = viewmodel_fov_y->get_int( );
		g_var.esp.view_z = viewmodel_fov_z->get_int( );
		g_var.esp.camdist = camdist->get_int( );
		do_once = true;
	}
	if ( viewmodel_fov->get_int( ) != g_var.esp.viewfov )
		viewmodel_fov->set_value( g_var.esp.viewfov );

	if ( viewmodel_fov_x->get_int( ) != g_var.esp.view_x )
		viewmodel_fov_x->set_value( g_var.esp.view_x );

	if ( viewmodel_fov_y->get_int( ) != g_var.esp.view_y )
		viewmodel_fov_y->set_value( g_var.esp.view_y );

	if ( viewmodel_fov_z->get_int( ) != g_var.esp.view_z )
		viewmodel_fov_z->set_value( g_var.esp.view_z );


	IDirect3DStateBlock9* pixel_state = nullptr;
	IDirect3DVertexDeclaration9* vertex_decleration = nullptr;
	IDirect3DVertexShader9* vertex_shader = nullptr;

	device->CreateStateBlock( D3DSBT_PIXELSTATE , &pixel_state );
	device->GetVertexDeclaration( &vertex_decleration );
	device->GetVertexShader( &vertex_shader );

	pixel_state->Capture( );

	if ( !render::initialized )
		if ( render::initialize( device ) )
			render::initialized = true;

	render::set_states( );
	features::visuals::main( );
	features::misc::draw( );
	features::events::events.log_events( );
	menu::endscene( );

	pixel_state->Apply( );
	pixel_state->Release( );
	device->SetVertexDeclaration( vertex_decleration );
	device->SetVertexShader( vertex_shader );

	interfaces::input_system->enable_input( !( menu::opened && ( !interfaces::engine->is_in_game( ) || !interfaces::engine->is_connected( ) ) ) );

	return endscene_fn( edx , ecx , device );
}