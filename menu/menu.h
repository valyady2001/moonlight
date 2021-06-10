#pragma once
#include <vars.h>
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"
#include "custom_font.h"
namespace menu {
	inline HWND WindowHandle = nullptr;
	inline WNDPROC oWndProcHook = nullptr;
	void endscene( );

	void ragebot( );
	void antihit( );
	void esp( );
	void misc( );

	LRESULT WndProcHook( HWND hwnd , UINT msg , WPARAM wparam , LPARAM lparam );

	void initialize( );
	void shutdown( );
	inline bool opened;
	inline bool init_menu = false;
	inline ImFont* tabs;

	inline void apply_fonts( ) {
		ImGui::CreateContext( );
		tabs = ImGui::GetIO( ).Fonts->AddFontFromMemoryCompressedBase85TTF( compressed_data_base85 , 36 );
		ImGui::GetIO( ).ButtonFont = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Verdana.ttf" , 24 );

		ImGui::GetIO( ).FontDefault = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Calibri.ttf" , 14 );
		ImGui::GetIO( ).CustomFont = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Calibri.ttf" , 12 );
	}

	inline void set_theme( )
	{
		ImGuiStyle& style = ImGui::GetStyle( );
		ImVec4* colors = style.Colors;
		style.WindowRounding = 0;
		style.ScrollbarSize = 4;
		style.WindowPadding = ImVec2( 4 , 20 );
		style.Colors[ ImGuiCol_Text ] = ImVec4( 1.f , 1.f , 1.f , 1.00f );
		style.Colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.60f , 0.60f , 0.60f , 1.00f );
		style.Colors[ ImGuiCol_WindowBg ] = ImVec4( 0.10f , 0.10f , 0.10f , 1.00f );
		style.Colors[ ImGuiCol_ChildBg ] = ImVec4( 0.13f , 0.13f , 0.13f , 1.00f );
		style.Colors[ ImGuiCol_PopupBg ] = ImVec4( 0.05f , 0.05f , 0.10f , 1.0f );
		style.Colors[ ImGuiCol_Border ] = ImVec4( 0.70f , 0.70f , 0.70f , 0.65f );
		style.Colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f , 0.00f , 0.00f , 0.00f );
		style.Colors[ ImGuiCol_FrameBg ] = ImVec4( 0.2f , 0.2f , 0.2f , 1.00f );   // Background of checkbox, radio button, plot, slider, text input
		style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.25f , 0.25f , 0.245f , 1.f );
		style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.25f , 0.25f , 0.25f , 1.0f );
		style.Colors[ ImGuiCol_TitleBg ] = ImVec4( 0.27f , 0.27f , 0.54f , 0.83f );
		style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.40f , 0.40f , 0.80f , 0.20f );
		style.Colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.32f , 0.32f , 0.63f , 0.87f );
		style.Colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.40f , 0.40f , 0.55f , 0.80f );
		style.Colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.20f , 0.25f , 0.30f , 1.00f );
		style.Colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.50f , 0.30f , 0.78f , 1.0f );
		style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.50f , 0.30f , 0.78f , 1.0f );
		style.Colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.50f , 0.30f , 0.78f , 1.0f );
		style.Colors[ ImGuiCol_CheckMark ] = ImVec4( 0.50f , 0.30f , 0.78f , 1.00f );
		style.Colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.50f , 0.30f , 0.78f , 0.30f );
		style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.50f , 0.30f , 0.78f , 0.30f );
		style.Colors[ ImGuiCol_Button ] = ImVec4( 0.13f , 0.13f , 0.13f , 1.00f );
		style.Colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.16f , 0.16f , 0.16f , 1.f );
		style.Colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.16f , 0.16f , 0.16f , 1.00f );
		style.Colors[ ImGuiCol_Header ] = ImVec4( 0.50f , 0.30f , 0.78f , 0.45f );
		style.Colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.50f , 0.30f , 0.78f , 0.7f );
		style.Colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.50f , 0.30f , 0.78f , 0.7f );
		style.Colors[ ImGuiCol_Separator ] = ImVec4( .52f , 0.f , 0.52f , 1.f );
		style.Colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.70f , 0.60f , 0.60f , 1.00f );
		style.Colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.90f , 0.70f , 0.70f , 1.00f );
		style.Colors[ ImGuiCol_ResizeGrip ] = ImVec4( 1.00f , 1.00f , 1.00f , 0.30f );
		style.Colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 1.00f , 1.00f , 1.00f , 0.60f );
		style.Colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 1.00f , 1.00f , 1.00f , 0.90f );
		//style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
		//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
		//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
		style.Colors[ ImGuiCol_PlotLines ] = ImVec4( 1.00f , 1.00f , 1.00f , 1.00f );
		style.Colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 0.90f , 0.70f , 0.00f , 1.00f );
		style.Colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.90f , 0.70f , 0.00f , 1.00f );
		style.Colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f , 0.60f , 0.00f , 1.00f );
		style.Colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.00f , 0.00f , 1.00f , 0.35f );
		style.Colors[ ImGuiCol_ModalWindowDarkening ] = ImVec4( 0.20f , 0.20f , 0.20f , 0.35f );
	}
}