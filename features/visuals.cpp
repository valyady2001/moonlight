#include "features.h"

bool features::visuals::get_playerbox( base_entity* ent , box& in ) {
	vec3_t origin , min , max;
	vec2_t flb , brt , blb , frt , frb , brb , blt , flt;
	float left , top , right , bottom;
	origin = ent->origin( );

	min = ent->mins( ) + origin;
	max = ent->maxs( ) + origin;


	vec3_t points[ ] = {
		vec3_t( min.x, min.y, min.z ),
		vec3_t( min.x, max.y, min.z ),
		vec3_t( max.x, max.y, min.z ),
		vec3_t( max.x, min.y, min.z ),
		vec3_t( max.x, max.y, max.z ),
vec3_t( min.x , max.y , max.z ) ,
vec3_t( min.x , min.y , max.z ) ,
vec3_t( max.x , min.y , max.z )
	};


	if ( !math::w2s( points[ 3 ] , flb ) || !math::w2s( points[ 5 ] , brt )
		|| !math::w2s( points[ 0 ] , blb ) || !math::w2s( points[ 4 ] , frt )
		|| !math::w2s( points[ 2 ] , frb ) || !math::w2s( points[ 1 ] , brb )
		|| !math::w2s( points[ 6 ] , blt ) || !math::w2s( points[ 7 ] , flt ) )
		return false;

	vec2_t arr[ ] = { flb, brt, blb, frt, frb, brb, blt, flt };



	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for ( int i = 1; i < 8; i++ ) {
		if ( left > arr[ i ].x )
			left = arr[ i ].x;
		if ( bottom < arr[ i ].y )
			bottom = arr[ i ].y;
		if ( right < arr[ i ].x )
			right = arr[ i ].x;
		if ( top > arr[ i ].y )
			top = arr[ i ].y;
	}

	in.x = ( int ) left;
	in.y = ( int ) top;
	in.w = int( right - left );
	in.h = int( bottom - top );


	return true;
}


color HealthGreenRedGradient( int32_t Health )
{
	Health = max( 0 , min( Health , 100 ) );
	return color( min( ( 510 * ( 100 - Health ) ) / 100 , 255 ) , min( ( 510 * Health ) / 100 , 255 ) , 0 );
}
namespace features
{
	namespace visuals
	{
		void aa_indicator( vec2_t screen )
		{
			if ( !g::local || !g::local->is_alive( ) )
				return;
			if ( !g_var.antihit.enable || !g_var.antihit.manual )
			{
				return;
			}
			if ( antiaim::side == freestanding_aa )
			{
				return;
			}
			switch ( antiaim::side )
			{
			case left_aa:
				auto size2 = render::text_size( "<" , render::fonts::unicode );
				render::text( { ( screen.x - screen.x / 2 - size2.x / 2 ) - 40,( screen.y - screen.y / 2 ) - 12 } , "<" , render::fonts::unicode , color( 0 , 255 , 187 , 200 ) );
				break;
			case right_aa:
				auto size3 = render::text_size( ">" , render::fonts::unicode );
				render::text( { ( screen.x - screen.x / 2 - size3.x / 2 ) + 40,( screen.y - screen.y / 2 ) - 12 } , ">" , render::fonts::unicode , color( 0 , 255 , 187 , 200 ) );
				break;
			}
		}


		void main( )
		{
			if ( !interfaces::engine->is_in_game( ) || !interfaces::engine->is_connected( ) || !g::local )
				return;
			{
				int x , y;
				interfaces::engine->get_screen_size( x , y );
				aa_indicator( vec2_t( x , y ) );
				if ( g_var.ragebot.exploit > 0 && g::local->is_alive( ) )
				{
					auto line = time_to_ticks( fabsf( ( ( g::local->tick_base( ) + 15 - ( exploit::shift_amt + 1 ) ) * interfaces::global_vars->interval_per_tick ) - interfaces::global_vars->curtime ) );

					render::filled_rect( vec2_t( ( x - x / 2 ) , y - 40 ) , { line ,10 } , HealthGreenRedGradient( line * 4.75f ) );
					render::filled_rect( vec2_t( ( x - x / 2 ) , y - 40 ) , { -line ,10 } , HealthGreenRedGradient( line * 4.75f ) );
				}
				std::string dmg = "DMG: ";
				dmg += std::to_string( ( g_var.ragebot.override_dmg && keyhandler::auto_check( g_var.ragebot.dmg_bind[ 1 ] , g_var.ragebot.dmg_bind[ 0 ] ) ) ? g_var.ragebot.dmg_override : g_var.ragebot.min_dmg );
				render::text( vec2_t( ( x - x + 20 ) , y / 2 - 10 ) , dmg.c_str( ) , render::fonts::unicode );
				if ( keyhandler::auto_check( g_var.ragebot.forceb_bind[ 1 ] , g_var.ragebot.forceb_bind[ 0 ] ) )
				{
					std::string baim = "BAIM";
					render::text( vec2_t( ( x - x + 20 ) , y / 2 - 30 ) , baim.c_str( ) , render::fonts::unicode , color::green( ) );
				}
			}

			for ( int i = 1; i <= interfaces::ent_list->get_highest_index( ); i++ ) {
				base_entity* e = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
				if ( !e )
					continue;

				const auto model = e->mdl( );
				if ( !model )
					continue;

				studiohdr_t* hdr = interfaces::model_info->studio_model( model );

				if ( !hdr || !strstr( hdr->name , "thrown" ) && !strstr( hdr->name , "dropped" ) )
					continue;
				std::string entityName = hdr->name;

				vec2_t scr;
				vec3_t pos;
				pos = e->origin( );
				if ( math::w2s( pos , scr ) )
				{
					if ( entityName.find( "fraggrenade" ) != std::string::npos )
						render::text( scr , "GRENADE" , render::fonts::tahoma_bold , color::white( ) , render::textflag_centeredx | render::textflag_outline );
					if ( entityName.find( "smokegrenade" ) != std::string::npos )
						render::text( scr , "SMOKE" , render::fonts::tahoma_bold , color::white( ) , render::textflag_centeredx | render::textflag_outline );
					if ( entityName.find( "molotov" ) != std::string::npos || entityName.find( "incendiarygrenade" ) != std::string::npos )
						render::text( scr , "INCENDIARY" , render::fonts::tahoma_bold , color::white( ) , render::textflag_centeredx | render::textflag_outline );
					if ( entityName.find( "flashbang" ) != std::string::npos )
						render::text( scr , "FLASHBANG" , render::fonts::tahoma_bold , color::white( ) , render::textflag_centeredx | render::textflag_outline );
					if ( entityName.find( "decoy" ) != std::string::npos )
						render::text( scr , "DECOY" , render::fonts::tahoma_bold , color::white( ) , render::textflag_centeredx | render::textflag_outline );

				}
			}

			for ( int i = 1; i <= 64; i++ ) {
				base_entity* player = ( base_entity* ) interfaces::ent_list->get_client_entity( i );
				if ( !player ||
					player->dormant( ) ||
					!player->is_alive( ) ||
					player->team( ) == g::local->team( ) )
					continue;
				if ( !paint_data_valid[ i ] )
					continue;
				if ( !paint_weapondata[ i ] || !paint_weapon[ i ] ) continue;

				context.bbox = paint_bbox[ i ];
				player_info_t info;
				interfaces::engine->get_player_info( i , &info );

				if ( g_var.esp.box )
					draw_box( player , context.bbox.x , context.bbox.y , context.bbox.w , context.bbox.h , color( g_var.esp.box_clr[ 0 ] , g_var.esp.box_clr[ 1 ] , g_var.esp.box_clr[ 2 ] ) );
				if ( g_var.esp.name )
					draw_name( player , context.bbox.x , context.bbox.y , context.bbox.w , context.bbox.h , info.name , color( g_var.esp.name_clr[ 0 ] , g_var.esp.name_clr[ 1 ] , g_var.esp.name_clr[ 2 ] ) );
				if ( g_var.esp.health )
					draw_health( player , context.bbox.x , context.bbox.y , context.bbox.w , context.bbox.h , color( g_var.esp.health_clr[ 0 ] , g_var.esp.health_clr[ 1 ] , g_var.esp.health_clr[ 2 ] ) );
				if ( g_var.esp.wpn )
					draw_weapon_name( player , context.bbox.x , context.bbox.y , context.bbox.w , context.bbox.h , color( g_var.esp.wpn_clr[ 0 ] , g_var.esp.wpn_clr[ 1 ] , g_var.esp.wpn_clr[ 2 ] ) );
				draw_flags( player , context.bbox.x , context.bbox.y , context.bbox.w , context.bbox.h );
			}
		}

		void draw_box( base_entity* entity , int x , int y , int w , int h , color clr )
		{
			render::outline_rect( vec2_t( x , y ) , vec2_t( w , h ) , clr );
		}
		void draw_name( base_entity* entity , int x , int y , int w , int h , const char* info , color clr )
		{
			render::text( vec2_t( x + w / 2 , y - 14 ) , info , render::fonts::tahoma_bold , clr , render::textflag_centeredx | render::textflag_outline );
		}
		void draw_health( base_entity* entity , int x , int y , int w , int h , color clr )
		{
			int health = std::clamp( entity->health( ) , 0 , 100 );
			vec2_t text_size = render::text_size( std::to_string( health ).c_str( ) , render::fonts::smallest_pixel );

			render::filled_rect( vec2_t( x - 7 , y - 1 ) , vec2_t( 4 , h + 2 ) , color( 0 , 0 , 0 , 150 ) );
			render::filled_rect( vec2_t( x - 6 , y + ( h - h * ( std::clamp<int>( entity->health( ) , 0 , 100.f ) / 100.f ) ) ) , vec2_t( 2 , h * ( std::clamp<int>( entity->health( ) , 0 , 100 ) / 100.f ) - ( entity->health( ) >= 100 ? 0 : -1 ) ) , clr );
			if ( !( std::clamp<int>( entity->health( ) , 0 , 100.f ) >= 99 ) )
				render::text( vec2_t( x - 6 - text_size.y / 2 , y + ( h - h * ( std::clamp<int>( entity->health( ) , 0 , 100.f ) / 100.f ) ) - text_size.y / 2 ) , std::to_string( entity->health( ) ).c_str( ) , render::fonts::smallest_pixel , color( 255 , 255 , 255 , 255 ) );

		}

		void draw_weapon_name( base_entity* entity , int x , int y , int w , int h , color clr )
		{
			try
			{
				if ( !paint_weapon[ entity->idx( ) ] ) return;
				render::text( vec2_t( x + w / 2 , y + h ) , paint_weapon[ entity->idx( ) ]->weap_name( ).c_str( ) , render::fonts::tahoma_bold , clr , render::textflag_centeredx | render::textflag_outline );
			}
			catch ( std::exception& ec )
			{
				dbg_print( ec.what( ) );
				return;
			}
		}
		void draw_flags( base_entity* entity , int x , int y , int w , int h )
		{
			std::vector<std::pair<std::string , color>> flags;
			if ( g_var.esp.flags[ 0 ] )
			{
				if ( entity->is_scoped( ) )
					flags.push_back( std::pair<std::string , color>( std::string( "scoped" ) , color( 255 , 255 , 255 , 255 ) ) );
			}
			if ( g_var.esp.flags[ 1 ] )
			{
				if ( entity->armor( ) > 0 )
					flags.push_back( std::pair<std::string , color>( std::string( "" ).append( entity->armor_name( ) ) , color( 0 , 120 , 135 , 255 ) ) );
			}

			if ( g_var.esp.flags[ 2 ] && entity->money( ) )
				flags.push_back( std::pair<std::string , color>( std::string( "$" ).append( std::to_string( entity->money( ) ) ) , color( 17 , 105 , 17 , 255 ) ) );

			//flags.push_back( std::pair<std::string , color>( "update" , lagcomp::updated[ entity->idx( ) ] ? color( 0 , 255 , 0 , 255 ) : color( 255 , 0 , 0 , 255 ) ) );

			auto position = 0;
			for ( auto text : flags ) {
				render::text( vec2_t( x + w + 5 , y + position ) , text.first.c_str( ) , render::fonts::tahoma_bold , text.second , render::textflag_outline );
				position += 10;
			}
		}

		void remove_smoke( ) {

			if ( !g::local )
				return;

			static auto smoke_count = *reinterpret_cast< uint32_t** >( utilities::find_sig( "client.dll" , "A3 ? ? ? ? 57 8B CB" ) + 1 );

			static std::vector<const char*> smoke_materials = {
				"particle/vistasmokev1/vistasmokev1_fire",
				"particle/vistasmokev1/vistasmokev1_smokegrenade",
				"particle/vistasmokev1/vistasmokev1_emods",
				"particle/vistasmokev1/vistasmokev1_emods_impactdust"
			};

			for ( auto material_name : smoke_materials ) {
				material_t* smoke = interfaces::material_system->find_material( material_name , nullptr );
				if ( smoke )
				{
					smoke->increment_reference_count( );
					if ( g_var.esp.removals[ 4 ] )
						smoke->set_material_var_flag( material_var_no_draw , true );
					else
						smoke->set_material_var_flag( material_var_no_draw , false );
				}
			}
			*( int* ) smoke_count = 0;
		}

		void remove_flash( ) {
			if ( !g_var.esp.removals[ 3 ] )
				return;

			if ( !interfaces::engine->is_connected( ) && !interfaces::engine->is_in_game( ) )
				return;

			if ( !g::local )
				return;

			if ( g::local->flash_duration( ) > 0.0f )
				g::local->flash_duration( ) = 0.0f;
		}
		void show_impacts( )
		{
			if ( !g::local )
				events::bullet_info.clear( );

			if ( !g_var.esp.sv_impacts ||
				events::bullet_info.size( ) <= 0 )
				return;

			for ( size_t i = 0; i < events::bullet_info.size( ); i++ ) {
				if ( events::bullet_info[ i ].impact_pos == vec3_t( 0 , 0 , 0 ) )
					continue;

				interfaces::debug_overlay->box_overlay( events::bullet_info[ i ].impact_pos , vec3_t( -2 , -2 , -2 ) , vec3_t( 2 , 2 , 2 ) , vec3_t( 0 , 0 , 0 ) , 0 , 0 , 255 , 125 , 4.f );
				events::bullet_info.erase( events::bullet_info.begin( ) + i );
			}
		}
		void color_modulate_world( ) {
			static auto draw_specific_static_prop = interfaces::cvar->find_var( "r_drawspecificstaticprop" );
			if ( draw_specific_static_prop->get_int( ) != 0 )
				draw_specific_static_prop->set_value( 0 );
			auto night_val = g_var.esp.nightmode / 100.f;
			night_val = std::clamp<float>( night_val , 0 , 0.97f );
			for ( MaterialHandle_t i = interfaces::material_system->first_material( ); i != interfaces::material_system->invalid_material_handle( ); i = interfaces::material_system->next_material( i ) )
			{
				auto material = interfaces::material_system->get_material( i );
				if ( !material )
					continue;

				if ( strstr( material->get_texture_group_name( ) , "World" ) || strstr( material->get_texture_group_name( ) , "StaticProp" ) )
				{
					if ( strstr( material->get_texture_group_name( ) , "StaticProp" ) )
						material->color_modulate( 1.f - night_val + 0.35f , 1.f - night_val + 0.35f , 1.f - night_val + 0.35f );
					else
						material->color_modulate( 1.f - night_val , 1.f - night_val , 1.f - night_val );

				}
			}
		}
	}
}