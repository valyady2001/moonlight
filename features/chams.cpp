#include "features.h"


void modulate( float color[ 4 ] , material_t* material )
{
	if ( material )
	{
		material->alpha_modulate( color[ 3 ] );
		material->color_modulate( color[ 0 ] , color[ 1 ] , color[ 2 ] );

		bool found_tint;
		const auto tint = material->find_var( "$envmaptint" , &found_tint );
		if ( found_tint && tint )
			tint->set_vec3_t( color[ 0 ] , color[ 1 ] , color[ 2 ] );

		if ( g_var.esp.chams.material_type == 3 || g_var.esp.chams.desync_mat_type == 3 ) {
			bool found_tint; auto tint = material->find_var( "$envmapfresnelminmaxexp" , &found_tint );
			if ( found_tint && tint ) {
				tint->set_vec3_t( 0.f , 1.f , 1.f );
			}
		}
	}
}

namespace features {
	namespace chams {

		void init( )
		{
			textured = interfaces::material_system->find_material( "debug/debugambientcube" , nullptr );
			textured->increment_reference_count( );
			flat = interfaces::material_system->find_material( "debug/debugdrawflat" , nullptr );
			flat->increment_reference_count( );
			glow_cover = interfaces::material_system->find_material( "dev/glow_armsrace" , nullptr );
			glow_cover->increment_reference_count( );
			bubble = interfaces::material_system->find_material( "models/inventory_items/cologne_prediction/cologne_prediction_glass" , nullptr );
			bubble->increment_reference_count( );
		}

		void run( decltype( &hook::draw_model_execute ) ofunc , void* ctx , void* state , const mdlrender_info_t& info , matrix_t* bone_to_world )
		{
			if ( interfaces::engine->is_in_game( ) && interfaces::engine->is_connected( ) && g::local ) {
				bool is_player = strstr( interfaces::model_info->model_name( info.model ) , "models/player" ) != nullptr;
				bool is_arm = strstr( interfaces::model_info->model_name( info.model ) , "arms" ) != nullptr;
				bool is_sleeve = strstr( interfaces::model_info->model_name( info.model ) , "sleeve" ) != nullptr;
				bool is_weapon = strstr( interfaces::model_info->model_name( info.model ) , "weapons/v_" ) != nullptr;

				auto normal_mat = [ & ]( int type = 0 )
				{
					if ( type == 0 )
						return textured;
					else if ( type == 1 )
						return flat;
					else if ( type == 2 )
						return bubble;
					else
						return flat;
				};

				if ( is_player )
				{
					auto entity = ( base_entity* ) ( interfaces::ent_list->get_client_entity( info.entity_index ) );
					if ( g::local && entity && entity->is_alive( ) && !entity->dormant( ) )
					{
						const auto enemy = entity->team( ) != g::local->team( );
						
						if ( entity == g::local )
						{
							if ( g_var.esp.chams.desync_chams && features::animations::fake_state.m_got_fake_matrix ) {
								for ( auto& i : features::animations::fake_state.m_fake_matrix )
								{
									i[ 0 ][ 3 ] += info.origin.x;
									i[ 1 ][ 3 ] += info.origin.y;
									i[ 2 ][ 3 ] += info.origin.z;
								}

								interfaces::model_render->force_mat( normal_mat( g_var.esp.chams.desync_mat_type ) );
								normal_mat( g_var.esp.chams.desync_mat_type )->set_material_var_flag( material_var_ignorez , false );
								modulate( g_var.esp.chams.desync_chams_color , normal_mat( g_var.esp.chams.desync_mat_type ) );
								ofunc( interfaces::model_render , nullptr , ctx , state , info , features::animations::fake_state.m_fake_matrix );

								if ( g_var.esp.chams.desync_mat_type == 3 )
								{
									interfaces::model_render->force_mat( glow_cover );
									glow_cover->set_material_var_flag( material_var_ignorez , false );
									modulate( g_var.esp.chams.desync_overlay_color , glow_cover );
									ofunc( interfaces::model_render , nullptr , ctx , state , info , features::animations::fake_state.m_fake_matrix );
								}

								for ( auto& i : features::animations::fake_state.m_fake_matrix )
								{
									i[ 0 ][ 3 ] -= info.origin.x;
									i[ 1 ][ 3 ] -= info.origin.y;
									i[ 2 ][ 3 ] -= info.origin.z;
								}

								interfaces::model_render->force_mat( nullptr );
								ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );
							}
							if ( g_var.esp.chams.local_chams )
							{
								interfaces::model_render->force_mat( normal_mat( g_var.esp.chams.local_mat_type ) );
								normal_mat( g_var.esp.chams.local_mat_type )->set_material_var_flag( material_var_ignorez , false );
								modulate( g_var.esp.chams.local_chams_color , normal_mat( g_var.esp.chams.local_mat_type ) );
								ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );

								if ( g_var.esp.chams.local_mat_type == 3 )
								{
									interfaces::model_render->force_mat( glow_cover );
									glow_cover->set_material_var_flag( material_var_ignorez , false );
									modulate( g_var.esp.chams.local_overlay_color , glow_cover );
									ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );
								}
							}
						}
						if ( enemy )
						{
							/*if ( lagcomp::rec[ entity->idx( ) ].size( ) > 0 )
							{
								auto last_rec = &lagcomp::rec[ entity->idx( ) ].back( );
								if ( last_rec )
								{
									float clr[ 4 ] = { 0.f,0.f,1.f,0.4f };
									interfaces::model_render->force_mat( flat );
									flat->set_material_var_flag( material_var_ignorez , false );
									modulate( clr , flat );
									ofunc( interfaces::model_render , nullptr , ctx , state , info , last_rec->left );
									interfaces::model_render->force_mat( nullptr );

									float clr2[ 4 ] = { 1.f,0.f,0.f,0.4f };
									interfaces::model_render->force_mat( flat );
									flat->set_material_var_flag( material_var_ignorez , false );
									modulate( clr2 , flat );
									ofunc( interfaces::model_render , nullptr , ctx , state , info , last_rec->right );
									interfaces::model_render->force_mat( nullptr );
								}
							}*/

							if ( g_var.esp.chams.hidden_chams )
							{
								interfaces::model_render->force_mat( normal_mat( g_var.esp.chams.material_type ) );
								normal_mat( g_var.esp.chams.material_type )->set_material_var_flag( material_var_ignorez , true );
								modulate( g_var.esp.chams.hidden_chams_color , normal_mat( g_var.esp.chams.material_type ) );
								ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );

								if ( g_var.esp.chams.material_type == 3 )
								{
									interfaces::model_render->force_mat( glow_cover );
									glow_cover->set_material_var_flag( material_var_ignorez , true );
									modulate( g_var.esp.chams.overlay_color , glow_cover );
									ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );
								}
							}

							if ( g_var.esp.chams.visible_chams )
							{
								interfaces::model_render->force_mat( normal_mat( g_var.esp.chams.material_type ) );
								normal_mat( g_var.esp.chams.material_type )->set_material_var_flag( material_var_ignorez , false );
								modulate( g_var.esp.chams.visible_chams_color , normal_mat( g_var.esp.chams.material_type ) );
								ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );

								if ( g_var.esp.chams.material_type == 3 )
								{
									interfaces::model_render->force_mat( glow_cover );
									glow_cover->set_material_var_flag( material_var_ignorez , false );
									modulate( g_var.esp.chams.overlay_color , glow_cover );
									ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );
								}
							}

						}
						else
						{
							if ( g_var.esp.removals[ 8 ] )
								interfaces::model_render->force_mat( nullptr );
						}
					}
				}
				ofunc( interfaces::model_render , nullptr , ctx , state , info , bone_to_world );
				interfaces::model_render->force_mat( nullptr );
			}
		}
	}
}
