#include "menu.h"
#include <render/render.h>

namespace Custom
{
	static auto vec3_t_getter = [ ]( void* vec , int idx , const char** out_text )
	{
		auto& vec3_t = *static_cast< std::vector<std::string>* >( vec );
		if ( idx < 0 || idx >= static_cast< int >( vec3_t.size( ) ) ) { return false; }
		*out_text = vec3_t.at( idx ).c_str( );
		return true;
	};

	bool Combo( const char* label , int* currIndex , std::vector<std::string>& values )
	{
		if ( values.empty( ) ) { return false; }
		return ImGui::Combo( label , currIndex , vec3_t_getter ,
			static_cast< void* >( &values ) , values.size( ) );
	}

	bool ListBox( const char* label , int* currIndex , std::vector<std::string>& values )
	{
		if ( values.empty( ) ) { return false; }
		return ImGui::ListBox( label , currIndex , vec3_t_getter ,
			static_cast< void* >( &values ) , values.size( ) );
	}
}

const char* menu_tab_name[ ]
{
	"C",
	"B",
	"D",
	"A"
};

const char* menu_tab_name2[ ]
{
	"Aimbot",
	"Anti-hit",
	"Visuals",
	"Misc"
};

void menu::endscene( )
{
	if ( !init_menu ) {
		ImGui::CreateContext( );

		ImGui_ImplWin32_Init( FindWindowA( "Valve001" , nullptr ) );
		ImGui_ImplDX9_Init( render::device );

		init_menu = true;
	}

	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );
	ImGuiStyle& style = ImGui::GetStyle( );
	const auto _window_pad = style.WindowPadding;
	const auto _spacing = style.ItemSpacing;
	const auto _button_clr = style.Colors[ ImGuiCol_Button ];
	const auto _child_clr = style.Colors[ ImGuiCol_ChildBg ];
	static int tab = 0;

	if ( opened ) {
		ImGui::SetNextWindowSize( ImVec2( 600 , 450 ) , ImGuiCond_Always );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding , ImVec2( 0 , 0 ) );
		ImGui::Begin( "Moonlight.club" , &opened , ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar );
		{

			ImGui::PopStyleVar( );
			ImGui::BeginAnimChild( "##tabs" , ImVec2( 120 , -1 ) );
			{
				style.ItemSpacing = ImVec2( 0 , 0 );

				ImGui::PushStyleColor( ImGuiCol_Text , ImVec4( 1.f , 1.f , 1.f , g_anim.infos[ "##tabs" ]->alpha > 0.f ? g_anim.infos[ "##tabs" ]->alpha : g_anim.infos[ "##tabs" ]->alpha2 ) );
				if(GImGui != NULL)
				ImGui::PushFont( g_anim.infos[ "##tabs" ]->alpha > 0.f ? ImGui::GetIO( ).FontDefault : tabs );

				for ( int i = 0; i < ARRAYSIZE( menu_tab_name ); i++ ) {
					style.Colors[ ImGuiCol_Button ] = i == tab ? style.Colors[ ImGuiCol_ButtonActive ] : _button_clr;

					if ( ImGui::Button( g_anim.infos[ "##tabs" ]->alpha > 0 ? menu_tab_name2[ i ] : menu_tab_name[ i ] , false , ImVec2( -1 , 60 ) ) ) tab = i;
				}

				ImGui::PopFont( );

				ImGui::PopStyleColor( );

				style.Colors[ ImGuiCol_Button ] = _button_clr;
				style.ItemSpacing = _spacing;

			}ImGui::EndChild( );
			ImGui::SameLine( );
			ImGui::SetCursorPosY( 8 );

			switch ( tab )
			{
			case 0:ragebot( );
				break;
			case 1:antihit( );
				break;
			case 2:esp( );
				break;
			case 3:misc( );
				break;
			}

		}
		ImGui::End( );
	}
	ImGui::EndFrame( );
	ImGui::Render( );
	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
}
const char* safepoint[ ] =
{
	"Normal",
	"Aggressive"
};
const char* hitboxes[ ] =
{
	"Head",
	"Chest",
	"Stomach",
	"Pelvis",
	"Arms",
	"Legs",
	"Foot"
};
const char* baim[ ] =
{
	"Slow walk",
	"Lethal",
	"Inaccurate desync"
};
const char* autostop[ ] =
{
	"None",
	"Slide",
	"Full"
};
const char* select_target[ ] =
{
	"FOV",
	"Health",
	"Distance"
};
const char* exploit[ ] =
{
	"None",
	"Doubletap"
};

const char* weapon_config[ ] =
{
	"AWP",
	"Scout",
	"AutoSniper",
	"Heavy Pistols",
	"Light Pistols",
	"SMGs",
	"Rifles"
};
void menu::ragebot( )
{
	ImGui::BeginChild( "MAIN##rage" , ImVec2( 275 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f , 170 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding );
	{
		ImGui::Checkbox( "Enable##rage" , &g_var.ragebot.enable );
		ImGui::Checkbox( "Auto scope" , &g_var.ragebot.autoscope );
		ImGui::Checkbox( "Auto fire" , &g_var.ragebot.autofire );
		ImGui::Checkbox( "Resolver" , &g_var.ragebot.resolver );
		ImGui::Combo( "Target selection", &g_var.target_selection, select_target, ARRAYSIZE( select_target ) );//trebuie pus
		ImGui::Combo( "Weapon", &g_var.weapon_config, weapon_config, ARRAYSIZE( weapon_config ) );//trebuie pus
	}
	ImGui::EndChild( true );
	ImGui::SameLine( );
	switch (g_var.weapon_config) {
	case 0:
		ImGui::BeginChild( "TARGET##rage", ImVec2( -1, 248 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Safepoint", &g_var.awp.safepoint_type, safepoint, ARRAYSIZE( safepoint ) );//trebuie pus
			ImGui::Slider( "Minimum damage:", &g_var.awp.min_dmg, 0, 120, true );
			ImGui::Checkbox( "Damage Override", &g_var.awp.override_dmg );
			if (g_var.awp.override_dmg)
			{
				ImGui::Slider( "Override damage:", &g_var.awp.dmg_override, 0, 120, true );
				ImGui::Keybind( "Override key", &g_var.awp.dmg_bind [ 0 ], &g_var.awp.dmg_bind [ 1 ] );
			}
			if (ImGui::BeginCombo( "Hitboxes", "Head" ))
			{
				for (int i = 0; i < 7; i++)
				{
					ImGui::Selectable( hitboxes [ i ], &g_var.awp.hitboxes [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			if (ImGui::BeginCombo( "Baim condition", "Slow walk" ))
			{
				for (int i = 0; i < 3; i++)
				{
					ImGui::Selectable( baim [ i ], &g_var.awp.baim [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			ImGui::Keybind( "Force baim", &g_var.awp.forceb_bind [ 0 ], &g_var.awp.forceb_bind [ 1 ] );
		}ImGui::EndChild( true );

		ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "ACCURACY##rage", ImVec2( 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f, 255 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Slider( "Hitchance:", &g_var.awp.hitchance, 0, 100 );
			ImGui::Checkbox( "Refine shot", &g_var.awp.refine_shot );//trebuie terminat
			ImGui::Combo( "Autostop mode", &g_var.awp.autostop_mode, autostop, ARRAYSIZE( autostop ) );

		}
		ImGui::EndChild( true );
		ImGui::SetCursorPosY( 240 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f );
		ImGui::BeginChild( "EXPLOITS##rage", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Exploit", &g_var.awp.exploit, exploit, ARRAYSIZE( exploit ) );
			ImGui::Keybind( "Exploit key", &g_var.awp.exploit_bind [ 0 ], &g_var.awp.exploit_bind [ 1 ] );
			ImGui::Checkbox( "Teleport", &g_var.teleport );

		}
		ImGui::EndChild( true );
		break;

	case 1:
		ImGui::BeginChild( "TARGET##rage", ImVec2( -1, 248 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Safepoint", &g_var.scout.safepoint_type, safepoint, ARRAYSIZE( safepoint ) );//trebuie pus
			ImGui::Slider( "Minimum damage:", &g_var.scout.min_dmg, 0, 120, true );
			ImGui::Checkbox( "Damage Override", &g_var.scout.override_dmg );
			if (g_var.scout.override_dmg)
			{
				ImGui::Slider( "Override damage:", &g_var.scout.dmg_override, 0, 120, true );
				ImGui::Keybind( "Override key", &g_var.scout.dmg_bind [ 0 ], &g_var.scout.dmg_bind [ 1 ] );
			}
			if (ImGui::BeginCombo( "Hitboxes", "Head" ))
			{
				for (int i = 0; i < 7; i++)
				{
					ImGui::Selectable( hitboxes [ i ], &g_var.scout.hitboxes [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			if (ImGui::BeginCombo( "Baim condition", "Slow walk" ))
			{
				for (int i = 0; i < 3; i++)
				{
					ImGui::Selectable( baim [ i ], &g_var.scout.baim [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			ImGui::Keybind( "Force baim", &g_var.scout.forceb_bind [ 0 ], &g_var.scout.forceb_bind [ 1 ] );
		}ImGui::EndChild( true );

		ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "ACCURACY##rage", ImVec2( 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f, 255 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Slider( "Hitchance:", &g_var.scout.hitchance, 0, 100 );
			ImGui::Checkbox( "Refine shot", &g_var.scout.refine_shot );//trebuie terminat
			ImGui::Combo( "Autostop mode", &g_var.scout.autostop_mode, autostop, ARRAYSIZE( autostop ) );

		}
		ImGui::EndChild( true );
		ImGui::SetCursorPosY( 240 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f );
		ImGui::BeginChild( "EXPLOITS##rage", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Exploit", &g_var.scout.exploit, exploit, ARRAYSIZE( exploit ) );
			ImGui::Keybind( "Exploit key", &g_var.scout.exploit_bind [ 0 ], &g_var.scout.exploit_bind [ 1 ] );
			ImGui::Checkbox( "Teleport", &g_var.teleport );

		}
		ImGui::EndChild( true );
		break;

	case 2:
		ImGui::BeginChild( "TARGET##rage", ImVec2( -1, 248 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Safepoint", &g_var.autosniper.safepoint_type, safepoint, ARRAYSIZE( safepoint ) );//trebuie pus
			ImGui::Slider( "Minimum damage:", &g_var.autosniper.min_dmg, 0, 120, true );
			ImGui::Checkbox( "Damage Override", &g_var.autosniper.override_dmg );
			if (g_var.autosniper.override_dmg)
			{
				ImGui::Slider( "Override damage:", &g_var.autosniper.dmg_override, 0, 120, true );
				ImGui::Keybind( "Override key", &g_var.autosniper.dmg_bind [ 0 ], &g_var.autosniper.dmg_bind [ 1 ] );
			}
			if (ImGui::BeginCombo( "Hitboxes", "Head" ))
			{
				for (int i = 0; i < 7; i++)
				{
					ImGui::Selectable( hitboxes [ i ], &g_var.autosniper.hitboxes [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			if (ImGui::BeginCombo( "Baim condition", "Slow walk" ))
			{
				for (int i = 0; i < 3; i++)
				{
					ImGui::Selectable( baim [ i ], &g_var.autosniper.baim [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			ImGui::Keybind( "Force baim", &g_var.autosniper.forceb_bind [ 0 ], &g_var.autosniper.forceb_bind [ 1 ] );
		}ImGui::EndChild( true );

		ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "ACCURACY##rage", ImVec2( 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f, 255 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Slider( "Hitchance:", &g_var.autosniper.hitchance, 0, 100 );
			ImGui::Checkbox( "Refine shot", &g_var.autosniper.refine_shot );//trebuie terminat
			ImGui::Combo( "Autostop mode", &g_var.autosniper.autostop_mode, autostop, ARRAYSIZE( autostop ) );

		}
		ImGui::EndChild( true );
		ImGui::SetCursorPosY( 240 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f );
		ImGui::BeginChild( "EXPLOITS##rage", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Exploit", &g_var.autosniper.exploit, exploit, ARRAYSIZE( exploit ) );
			ImGui::Keybind( "Exploit key", &g_var.autosniper.exploit_bind [ 0 ], &g_var.autosniper.exploit_bind [ 1 ] );
			ImGui::Checkbox( "Teleport", &g_var.teleport );

		}
		ImGui::EndChild( true );
		break;

	case 3:
		ImGui::BeginChild( "TARGET##rage", ImVec2( -1, 248 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Safepoint", &g_var.heavy.safepoint_type, safepoint, ARRAYSIZE( safepoint ) );//trebuie pus
			ImGui::Slider( "Minimum damage:", &g_var.heavy.min_dmg, 0, 120, true );
			ImGui::Checkbox( "Damage Override", &g_var.heavy.override_dmg );
			if (g_var.heavy.override_dmg)
			{
				ImGui::Slider( "Override damage:", &g_var.heavy.dmg_override, 0, 120, true );
				ImGui::Keybind( "Override key", &g_var.heavy.dmg_bind [ 0 ], &g_var.heavy.dmg_bind [ 1 ] );
			}
			if (ImGui::BeginCombo( "Hitboxes", "Head" ))
			{
				for (int i = 0; i < 7; i++)
				{
					ImGui::Selectable( hitboxes [ i ], &g_var.heavy.hitboxes [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			if (ImGui::BeginCombo( "Baim condition", "Slow walk" ))
			{
				for (int i = 0; i < 3; i++)
				{
					ImGui::Selectable( baim [ i ], &g_var.heavy.baim [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			ImGui::Keybind( "Force baim", &g_var.heavy.forceb_bind [ 0 ], &g_var.heavy.forceb_bind [ 1 ] );
		}ImGui::EndChild( true );

		ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "ACCURACY##rage", ImVec2( 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f, 255 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Slider( "Hitchance:", &g_var.heavy.hitchance, 0, 100 );
			ImGui::Checkbox( "Refine shot", &g_var.heavy.refine_shot );//trebuie terminat
			ImGui::Combo( "Autostop mode", &g_var.heavy.autostop_mode, autostop, ARRAYSIZE( autostop ) );

		}
		ImGui::EndChild( true );
		ImGui::SetCursorPosY( 240 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f );
		ImGui::BeginChild( "EXPLOITS##rage", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Exploit", &g_var.heavy.exploit, exploit, ARRAYSIZE( exploit ) );
			ImGui::Keybind( "Exploit key", &g_var.heavy.exploit_bind [ 0 ], &g_var.heavy.exploit_bind [ 1 ] );
			ImGui::Checkbox( "Teleport", &g_var.teleport );

		}
		ImGui::EndChild( true );
		break;

	case 4:
		ImGui::BeginChild( "TARGET##rage", ImVec2( -1, 248 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Safepoint", &g_var.pistols.safepoint_type, safepoint, ARRAYSIZE( safepoint ) );//trebuie pus
			ImGui::Slider( "Minimum damage:", &g_var.pistols.min_dmg, 0, 120, true );
			ImGui::Checkbox( "Damage Override", &g_var.pistols.override_dmg );
			if (g_var.pistols.override_dmg)
			{
				ImGui::Slider( "Override damage:", &g_var.pistols.dmg_override, 0, 120, true );
				ImGui::Keybind( "Override key", &g_var.pistols.dmg_bind [ 0 ], &g_var.pistols.dmg_bind [ 1 ] );
			}
			if (ImGui::BeginCombo( "Hitboxes", "Head" ))
			{
				for (int i = 0; i < 7; i++)
				{
					ImGui::Selectable( hitboxes [ i ], &g_var.pistols.hitboxes [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			if (ImGui::BeginCombo( "Baim condition", "Slow walk" ))
			{
				for (int i = 0; i < 3; i++)
				{
					ImGui::Selectable( baim [ i ], &g_var.pistols.baim [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			ImGui::Keybind( "Force baim", &g_var.pistols.forceb_bind [ 0 ], &g_var.pistols.forceb_bind [ 1 ] );
		}ImGui::EndChild( true );

		ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "ACCURACY##rage", ImVec2( 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f, 255 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Slider( "Hitchance:", &g_var.pistols.hitchance, 0, 100 );
			ImGui::Checkbox( "Refine shot", &g_var.pistols.refine_shot );//trebuie terminat
			ImGui::Combo( "Autostop mode", &g_var.pistols.autostop_mode, autostop, ARRAYSIZE( autostop ) );

		}
		ImGui::EndChild( true );
		ImGui::SetCursorPosY( 240 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f );
		ImGui::BeginChild( "EXPLOITS##rage", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Exploit", &g_var.pistols.exploit, exploit, ARRAYSIZE( exploit ) );
			ImGui::Keybind( "Exploit key", &g_var.pistols.exploit_bind [ 0 ], &g_var.pistols.exploit_bind [ 1 ] );
			ImGui::Checkbox( "Teleport", &g_var.teleport );

		}
		ImGui::EndChild( true );
		break;

	case 5:
		ImGui::BeginChild( "TARGET##rage", ImVec2( -1, 248 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Safepoint", &g_var.smgs.safepoint_type, safepoint, ARRAYSIZE( safepoint ) );//trebuie pus
			ImGui::Slider( "Minimum damage:", &g_var.smgs.min_dmg, 0, 120, true );
			ImGui::Checkbox( "Damage Override", &g_var.smgs.override_dmg );
			if (g_var.smgs.override_dmg)
			{
				ImGui::Slider( "Override damage:", &g_var.smgs.dmg_override, 0, 120, true );
				ImGui::Keybind( "Override key", &g_var.smgs.dmg_bind [ 0 ], &g_var.smgs.dmg_bind [ 1 ] );
			}
			if (ImGui::BeginCombo( "Hitboxes", "Head" ))
			{
				for (int i = 0; i < 7; i++)
				{
					ImGui::Selectable( hitboxes [ i ], &g_var.smgs.hitboxes [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			if (ImGui::BeginCombo( "Baim condition", "Slow walk" ))
			{
				for (int i = 0; i < 3; i++)
				{
					ImGui::Selectable( baim [ i ], &g_var.smgs.baim [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			ImGui::Keybind( "Force baim", &g_var.smgs.forceb_bind [ 0 ], &g_var.smgs.forceb_bind [ 1 ] );
		}ImGui::EndChild( true );

		ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "ACCURACY##rage", ImVec2( 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f, 255 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Slider( "Hitchance:", &g_var.smgs.hitchance, 0, 100 );
			ImGui::Checkbox( "Refine shot", &g_var.smgs.refine_shot );//trebuie terminat
			ImGui::Combo( "Autostop mode", &g_var.smgs.autostop_mode, autostop, ARRAYSIZE( autostop ) );

		}
		ImGui::EndChild( true );
		ImGui::SetCursorPosY( 240 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f );
		ImGui::BeginChild( "EXPLOITS##rage", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Exploit", &g_var.smgs.exploit, exploit, ARRAYSIZE( exploit ) );
			ImGui::Keybind( "Exploit key", &g_var.smgs.exploit_bind [ 0 ], &g_var.smgs.exploit_bind [ 1 ] );
			ImGui::Checkbox( "Teleport", &g_var.teleport );

		}
		ImGui::EndChild( true );
		break;

	case 6:
		ImGui::BeginChild( "TARGET##rage", ImVec2( -1, 248 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Safepoint", &g_var.rifles.safepoint_type, safepoint, ARRAYSIZE( safepoint ) );//trebuie pus
			ImGui::Slider( "Minimum damage:", &g_var.rifles.min_dmg, 0, 120, true );
			ImGui::Checkbox( "Damage Override", &g_var.rifles.override_dmg );
			if (g_var.rifles.override_dmg)
			{
				ImGui::Slider( "Override damage:", &g_var.rifles.dmg_override, 0, 120, true );
				ImGui::Keybind( "Override key", &g_var.rifles.dmg_bind [ 0 ], &g_var.rifles.dmg_bind [ 1 ] );
			}
			if (ImGui::BeginCombo( "Hitboxes", "Head" ))
			{
				for (int i = 0; i < 7; i++)
				{
					ImGui::Selectable( hitboxes [ i ], &g_var.rifles.hitboxes [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			if (ImGui::BeginCombo( "Baim condition", "Slow walk" ))
			{
				for (int i = 0; i < 3; i++)
				{
					ImGui::Selectable( baim [ i ], &g_var.rifles.baim [ i ], ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
				}
				ImGui::EndCombo( );
			}
			ImGui::Keybind( "Force baim", &g_var.rifles.forceb_bind [ 0 ], &g_var.rifles.forceb_bind [ 1 ] );
		}ImGui::EndChild( true );

		ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "ACCURACY##rage", ImVec2( 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f, 255 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Slider( "Hitchance:", &g_var.rifles.hitchance, 0, 100 );
			ImGui::Checkbox( "Refine shot", &g_var.rifles.refine_shot );//trebuie terminat
			ImGui::Combo( "Autostop mode", &g_var.rifles.autostop_mode, autostop, ARRAYSIZE( autostop ) );

		}
		ImGui::EndChild( true );
		ImGui::SetCursorPosY( 240 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos [ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos [ "##tabs" ]->child_wishsize.x / 2.5f );
		ImGui::BeginChild( "EXPLOITS##rage", ImVec2( -1, -1 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar, true );
		{
			ImGui::Combo( "Exploit", &g_var.rifles.exploit, exploit, ARRAYSIZE( exploit ) );
			ImGui::Keybind( "Exploit key", &g_var.rifles.exploit_bind [ 0 ], &g_var.rifles.exploit_bind [ 1 ] );
			ImGui::Checkbox( "Teleport", &g_var.teleport );

		}
		ImGui::EndChild( true );
		break;
	}
}
const char* fl[ ] =
{
	"Normal",
	"Random",
	"Adaptive"
};
const char* pitches[ ] =
{
	"Down",
	"Up",
	"Zero",
	"Fake up",
	"Fake down"
};
const char* desyncs[ ] =
{
	"None",
	"Inverter",
	"Swap Jitter",
	"Opposite",
	"Custom Jitter"
};

void menu::antihit( )
{
	ImGui::BeginChild( "ANTI-HIT##idk" , ImVec2( 275 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f , 180 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar );
	{
		ImGui::Checkbox( "Enable##antihit" , &g_var.antihit.enable );
		ImGui::Checkbox( "At target##target" , &g_var.antihit.at_target );
		ImGui::Checkbox( "Freestand##target" , &g_var.antihit.freestand );
		ImGui::Checkbox( "Manual##manual" , &g_var.antihit.manual );

		ImGui::Combo( "Desync type", &g_var.desync_type, desyncs, ARRAYSIZE( desyncs ) );
		if ( g_var.antihit.manual )
		{
			ImGui::Keybind( "Left" , &g_var.antihit.left_key , NULL );
			ImGui::Keybind( "Right" , &g_var.antihit.right_key , NULL );
		}
		if (g_var.desync_type == 1) {
			ImGui::Slider( "Lean Yaw", &g_var.general_body_yaw, 0, 100 );
			ImGui::Slider( "Inverted Lean Yaw", &g_var.general_inverse_body_yaw, 0, 100 );
			ImGui::Keybind( "Inverter", &g_var.antihit.inverter [ 0 ], &g_var.antihit.inverter [ 1 ] );
		}
		if (g_var.desync_type == 3) {
			ImGui::Slider( "Standing extend", &g_var.st_opposite_amt, 0, 100 );
			ImGui::Slider( "Moving extend", &g_var.mv_opposite_amt, 0, 100 );
			ImGui::Slider( "In air extend", &g_var.air_opposite_amt, 0, 100 );
		}
		if (g_var.desync_type == 4) { 
			ImGui::Slider( "Real Jitter", &g_var.real_jitter, 0, 29 );
			ImGui::Slider( "Fake Jitter", &g_var.fake_jitter, 0, 29 );
		}
	}
	ImGui::EndChild( true );
	ImGui::SameLine( );
	ImGui::BeginChild( "CUSTOMIZE##custom" , ImVec2( -1 , -1 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding , true );
	{
		if ( !g_var.misc.antiuntrusted )
			ImGui::Combo( "Pitch" , &g_var.antihit.pitch , pitches , ARRAYSIZE( pitches ) );

	}ImGui::EndChild( true );
		ImGui::SetCursorPosY( 160 + 16 );//last height + 2x padding
		ImGui::SetCursorPosX( g_anim.infos[ "##tabs" ]->child_wishsize.x + 8 );
		ImGui::BeginChild( "FAKELAG##fake" , ImVec2( 275 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f , 265 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding );
		{
			ImGui::Checkbox( "Enable##fakelag" , &g_var.antihit.enable_fl );
			ImGui::Combo( "Fakelag mode" , &g_var.antihit.type , fl , ARRAYSIZE( fl ) );
			switch ( g_var.antihit.type )
			{
			case 0:
				ImGui::Slider( "Standing choke" , &g_var.antihit.st_lag , 0 , 14 );
				ImGui::Slider( "Moving choke" , &g_var.antihit.mv_lag , 0 , 14 );
				ImGui::Slider( "In air choke" , &g_var.antihit.air_lag , 0 , 14 );
				break;
			case 1:
				ImGui::Slider( "Minimum choke" , &g_var.antihit.min_lag , 0 , 14 );
				ImGui::Slider( "Maximum choke" , &g_var.antihit.max_lag , 0 , 14 );
				break;
			case 2:
				ImGui::Slider( "Adaptive max choke" , &g_var.antihit.adaptive , 0 , 14 );
				break;
			}
		}
		ImGui::EndChild( true );
}
const char* hitsound[ ] =
{
	"None",
	"Bell",
	"Bubble",
	"Metal"
};
const char* flags[ ] =
{
	"Scope",
	"Armor",
	"Money"
};
const char* chams_overlay[ ] = {
	"None",
	"Glow cover",
	"Glow outline"
};
const char* mat_type[ ] = {
	"Material",
	"Flat",
	"Bubble",
	"Glow cover"
};

const char* chams_type[ ] = {
	"Enemies",
	"Local",
	"Fake yaw"
};
const char* removals[ ] = {
	"Zoom",
	"Scope",
	"Visual recoil",
	"Flash",
	"Smoke",
	"Postprocessing",
	"Panorama blur",
	"Shadows",
	"Teammates"
};
void menu::esp( )
{
	ImGui::BeginChild( "INFO##esp" , ImVec2( 275 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f , 200 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding );
	{
		ImGui::Checkbox( "Box" , &g_var.esp.box ); ImGui::SameLine( );
		ImGui::ColorEdit3( "##box" , g_var.esp.box_clr , ImGuiColorEditFlags_NoInputs );
		ImGui::Checkbox( "Name" , &g_var.esp.name ); ImGui::SameLine( );
		ImGui::ColorEdit3( "##name" , g_var.esp.name_clr , ImGuiColorEditFlags_NoInputs );
		ImGui::Checkbox( "Weapon name" , &g_var.esp.wpn ); ImGui::SameLine( );
		ImGui::ColorEdit3( "##wpn" , g_var.esp.wpn_clr , ImGuiColorEditFlags_NoInputs );
		ImGui::Checkbox( "Health" , &g_var.esp.health ); ImGui::SameLine( );
		ImGui::ColorEdit3( "##health" , g_var.esp.health_clr , ImGuiColorEditFlags_NoInputs );
		ImGui::Checkbox( "Glow" , &g_var.esp.glow ); ImGui::SameLine( );
		ImGui::ColorEdit4( "##Glow" , g_var.esp.glow_clr , ImGuiColorEditFlags_NoInputs );
		if ( ImGui::BeginCombo( "Flags" , "Scope" ) )
		{
			for ( int i = 0; i < 3; i++ )
			{
				ImGui::Selectable( flags[ i ] , &g_var.esp.flags[ i ] , ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
			}
			ImGui::EndCombo( );
		}
	}
	ImGui::EndChild( true );
	ImGui::SameLine( );
	ImGui::BeginChild( "MISC##esp" , ImVec2( -1 , -1 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding , true );
	{
		ImGui::Combo( "Hitsound" , &g_var.misc.sound , hitsound , ARRAYSIZE( hitsound ) );
		ImGui::Checkbox( "Kill effect" , &g_var.esp.kill_effect );
		ImGui::Checkbox( "Fullbright" , &g_var.esp.fullbright );
		ImGui::Checkbox( "Grenade prediction" , &g_var.esp.grenade_pred );
		ImGui::Checkbox( "Bullet impacts" , &g_var.esp.sv_impacts );
		ImGui::Slider( "Nightmode" , &g_var.esp.nightmode , 0 , 100 );
		if ( ImGui::BeginCombo( "Removals" , "Zoom" ) )
		{
			for ( int i = 0; i < 9; i++ )
			{
				ImGui::Selectable( removals[ i ] , &g_var.esp.removals[ i ] , ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
			}
			ImGui::EndCombo( );
		}
		ImGui::Slider( "Viewmodel fov" , &g_var.esp.viewfov , 60 , 120 );
		ImGui::Slider( "Viewmodel x" , &g_var.esp.view_x , -20 , 20 );
		ImGui::Slider( "Viewmodel y" , &g_var.esp.view_y , -20 , 20 );
		ImGui::Slider( "Viewmodel z" , &g_var.esp.view_z , -20 , 20 );
	}
	ImGui::EndChild( true );
	ImGui::SetCursorPosY( 200 + 16 );//last height + 2x padding
	ImGui::SetCursorPosX( g_anim.infos[ "##tabs" ]->child_wishsize.x + 8 );
	ImGui::BeginChild( "CHAMS##esp" , ImVec2( 283 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f , -1 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding , true );
	{
		ImGui::Combo( "chams-selection" , &g_var.esp.chams.chams_selection , chams_type , ARRAYSIZE( chams_type ) );
		switch ( g_var.esp.chams.chams_selection )
		{
		case 0:
			ImGui::Checkbox( "Visible##enemy" , &g_var.esp.chams.visible_chams );
			ImGui::SameLine( );
			ImGui::ColorEdit4( "###visiblecolor" , g_var.esp.chams.visible_chams_color , ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs );
			ImGui::Checkbox( "Hidden##enemy" , &g_var.esp.chams.hidden_chams );
			ImGui::SameLine( );
			ImGui::ColorEdit4( "###hiddencolor" , g_var.esp.chams.hidden_chams_color , ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs );
			ImGui::Combo( "Material##enemy" , &g_var.esp.chams.material_type , mat_type , ARRAYSIZE( mat_type ) );
			if ( g_var.esp.chams.material_type == 3 )
				ImGui::ColorEdit4( "Overlay##enemy" , g_var.esp.chams.overlay_color , ImGuiColorEditFlags_NoInputs );

			break;
		case 1:
			ImGui::Checkbox( "Enable##local" , &g_var.esp.chams.local_chams );
			ImGui::Combo( "Material##local" , &g_var.esp.chams.local_mat_type , mat_type , ARRAYSIZE( mat_type ) );
			ImGui::ColorEdit4( "Material color##local" , g_var.esp.chams.local_chams_color , ImGuiColorEditFlags_NoInputs );
			if ( g_var.esp.chams.desync_mat_type == 3 )
				ImGui::ColorEdit4( "Overlay##local" , g_var.esp.chams.local_overlay_color , ImGuiColorEditFlags_NoInputs );
			break;
		case 2:

			ImGui::Checkbox( "Enable##desync" , &g_var.esp.chams.desync_chams );
			ImGui::Combo( "Material##desync" , &g_var.esp.chams.desync_mat_type , mat_type , ARRAYSIZE( mat_type ) );
			ImGui::ColorEdit4( "Material color##desync" , g_var.esp.chams.desync_chams_color , ImGuiColorEditFlags_NoInputs );
			if ( g_var.esp.chams.desync_mat_type == 3 )
				ImGui::ColorEdit4( "Overlay##desync" , g_var.esp.chams.desync_overlay_color , ImGuiColorEditFlags_NoInputs );

			break;
		}
	}
	ImGui::EndChild( true );

}
const char* logs[ ] =
{
	"Buy",
	"Hurt",
	"Harmed",
	"Misses"
};
const char* primary[ ] =
{
	"None","Ak47/M4","G3SG1/SCAR20","Awp","Scout"
};
const char* secondary[ ] =
{
	"None","Duals","Deagle / R8","Five - seven / Tec9","P250"
};
const char* utilities[ ] =
{
	"Taser",
	"Armor",
	"Nades",
	"Defuser"
};
bool exists_test3( const std::string& name ) {
	struct stat buffer;
	return ( stat( name.c_str( ) , &buffer ) == 0 );
}

void menu::misc( )
{
	ImGui::BeginChild( "MISC##misc" , ImVec2( 275 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f , 170 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding );
	{
		ImGui::Keybind( "Thirdperson" , &g_var.misc.tp_bind[ 0 ] , &g_var.misc.tp_bind[ 1 ] );
		ImGui::Slider( "Distance" , &g_var.esp.camdist , 100 , 200 );
		ImGui::Checkbox( "Anti untrusted" , &g_var.misc.antiuntrusted );
		if ( ImGui::BeginCombo( "Event logs" , "Buy" ) )
		{
			for ( int i = 0; i < 4; i++ )
			{
				ImGui::Selectable( logs[ i ] , &g_var.misc.logs[ i ] , ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
			}
			ImGui::EndCombo( );
		}
	}
	ImGui::EndChild( true );
	ImGui::SameLine( );
	ImGui::BeginChild( "BUYBOT##misc" , ImVec2( -1 , 178 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar , true );
	{
		ImGui::Checkbox( "Enable##misc" , &g_var.misc.buybot.enable );
		ImGui::Combo( "Primary" , &g_var.misc.buybot.primary , primary , ARRAYSIZE( primary ) );
		ImGui::Combo( "Secondary" , &g_var.misc.buybot.secondary , secondary , ARRAYSIZE( secondary ) );
		if ( ImGui::BeginCombo( "Utility" , "Taser" ) )
		{
			for ( int i = 0; i < 4; i++ )
			{
				ImGui::Selectable( utilities[ i ] , &g_var.misc.buybot.utility[ i ] , ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups );
			}
			ImGui::EndCombo( );
		}

	}ImGui::EndChild( true );
	ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
	ImGui::SetCursorPosX( g_anim.infos[ "##tabs" ]->child_wishsize.x + 8 );
	ImGui::BeginChild( "MOVEMENT##misc" , ImVec2( 275 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f , 255 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding );
	{
		ImGui::Checkbox( "Bunnyhop" , &g_var.misc.bunnyhop );
		ImGui::Checkbox( "Strafer" , &g_var.misc.strafer );
		ImGui::Checkbox( "Clantag", &g_var.clantag );
		ImGui::Keybind( "Fakeduck" , &g_var.antihit.fakeduck_bind[ 0 ] , &g_var.antihit.fakeduck_bind[ 1 ] );
	}
	ImGui::EndChild( true );
	ImGui::SetCursorPosY( 170 + 16 );//last height + 2x padding
	ImGui::SetCursorPosX( g_anim.infos[ "##tabs" ]->child_wishsize.x + 16 + 275 - g_anim.infos[ "##tabs" ]->child_wishsize.x / 2.5f );
	ImGui::BeginChild( "CONFIG##misc" , ImVec2( -1 , -1 ) , false , ImGuiWindowFlags_AlwaysUseWindowPadding , true );
	{
		static char cfgname[ 64 ];
		ImGui::PushItemWidth( -1 );
		ImGui::InputText( "##name" , cfgname , 64 );
		if ( ImGui::Button( "Create" , false , ImVec2( -1 , 18 ) ) && !exists_test3( "C:\\moonlight\\" + std::string( cfgname ) + ".cfg" ) ) {
			g_var.create( cfgname );
			g_var.save( cfgname );
		}
		static std::string selected_config = "default";
		ImGui::ListBoxHeader( "##configs" , ImVec2( -1 , 100 ) ); {

			if ( g_var.getcfg( ).size( ) ) {
				for ( auto config : g_var.getcfg( ) ) {
					bool cacat = ( selected_config == config );
					if ( ImGui::Selectable( config.c_str( ) , &cacat ) ) selected_config = config;
				}
			}

			ImGui::ListBoxFooter( );
		}
		if ( ImGui::Button( "Save" , false , ImVec2( -1 , 18 ) ) ) g_var.save( selected_config );
		if ( ImGui::Button( "Load" , false , ImVec2( -1 , 18 ) ) ) { g_var.load( selected_config ); /*interfaces::client_state->force_full_update( );*/ }
		if ( ImGui::Button( "Delete" , false , ImVec2( -1 , 18 ) ) ) remove( std::string( "C:\\moonlight\\" + selected_config + ".cfg" ).c_str( ) );

	}
	ImGui::EndChild( true );
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

LRESULT menu::WndProcHook( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam ) {

	if ( msg == WM_KEYUP && wparam == VK_INSERT )
		opened = !opened;

	if ( opened ) {
		/* ImGui_ImplWin32_WndProcHandler usually returns false instead of true btw */
		if ( ImGui_ImplWin32_WndProcHandler( hwnd , msg , wparam , lparam ) )
			return true;

		/* ImGui Wants Text Input so... */
		if (GImGui != NULL)
		 if ( ImGui::GetIO( ).WantTextInput )
			return true;

		/* block game input for some messages */
		switch ( msg )
		{
			/* pressing mouse key */
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:

			/* scrolling */
		case WM_MOUSEWHEEL:

			/* after releasing mouse key */
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:

			/* when moving mouse */
		case WM_MOUSEMOVE:

			/* don't allow game input */
			return true;
		}
	}

	return CallWindowProcA( oWndProcHook , hwnd , msg , wparam , lparam );
}

void menu::initialize( ) {
	WindowHandle = FindWindowA( ( "Valve001" ) , nullptr );

	if ( WindowHandle )
		oWndProcHook = reinterpret_cast< WNDPROC >( SetWindowLongPtr( WindowHandle , GWLP_WNDPROC , reinterpret_cast< LONG_PTR >( WndProcHook ) ) );
}

void menu::shutdown( ) {
	SetWindowLongA( WindowHandle , GWLP_WNDPROC , long( oWndProcHook ) );
}