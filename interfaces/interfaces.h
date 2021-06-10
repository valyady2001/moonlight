#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include "engine.h"
#include "input_sys.h"
#include "input.h"
#include "prediction.h"
#include "trace.h"
#include "entlist.h"
#include "client.h"
#include "global_vars.h"
#include "model_render.h"
#include "model_info.h"
#include "material_sys.h"
#include "surface.h"
#include "convar.h"
#include "event_manager.h"
#include "malloc.h"
#include "render_beam.h"
#include "render_view.h"

class i_engine_sound;

namespace interfaces
{
	extern DWORD dev;
	extern iv_engine_client* engine;
	extern i_input_system* input_system;
	extern c_input* input;
	extern vinput* input2;
	extern player_prediction* prediction;
	extern player_move_helper* move_helper;
	extern player_game_movement* game_movement;
	extern clientstate* client_state;
	extern trace* trace_ray;
	extern physic_props* physics_props;
	extern c_entlist* ent_list;
	extern c_client* client;
	extern c_global_vars* global_vars;
	extern c_mdlrender* model_render;
	extern c_mdlinfo* model_info;
	extern c_matsys* material_system;
	extern c_surface* surface;
	extern c_cvar* cvar;
	extern i_game_event_manager2* event_manager;
	extern c_mem_alloc* mem_alloc;
	extern c_clientmode* clientmode;
	extern c_glow_manager* glow_manager;
	extern i_engine_sound* sound;
	extern player_resource* ent_resource;
	extern size_t PlayerAnimState;
	extern IViewRenderBeams* render_beams;
	extern i_render_view* render_view;
	extern i_panel* panel;
	extern iv_debug_overlay* debug_overlay;
	void init( );

}

