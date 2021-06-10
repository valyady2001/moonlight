#pragma once
#include "../sdk/classes/color.h"
#include "../sdk/math/vec3.h"
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#include <string_view>

namespace render {
	inline IDirect3DDevice9* device;

	inline bool initialized = false;

	struct vertex_t {
		float m_x, m_y, m_z, m_rhw;
		std::uint32_t m_color;
	};

	namespace fonts {
		inline ID3DXFont* segoeui;
		inline ID3DXFont* segoeui_bigger;
		inline ID3DXFont* tahoma_bold;
		inline ID3DXFont* smallest_pixel;
		inline ID3DXFont* verdana_small;
		inline ID3DXFont* unicode;
	}

	bool initialize(IDirect3DDevice9* Device);
	void reset();

	void set_states();

	D3DVIEWPORT9 get_viewport();
	void set_viewport(D3DVIEWPORT9 vp);

	D3DVIEWPORT9 start_clipping(vec2_t pos, vec2_t size);
	void end_clipping(D3DVIEWPORT9 vp);

	void outline_rect(vec2_t pos, vec2_t size, color clr);
	void filled_rect(vec2_t pos, vec2_t size, color clr);
	void filled_gradient_rect(vec2_t pos, vec2_t size, color clr1, color clr2, bool vertical = false);

	void line(vec2_t start, vec2_t end, color clr);

	enum textflags {
		textflag_none = 0,
		textflag_dropshadow = 1 << 0,
		textflag_outline = 1 << 1,
		textflag_centeredx = 1 << 2,
		textflag_centeredy = 1 << 3
	};

	vec2_t text_size(std::string_view txt, ID3DXFont* font);
	void text(vec2_t pos, std::string_view output, LPD3DXFONT font, color clr = color(255, 255, 255, 255), int flags = textflags::textflag_none);
	void text_unicode( vec2_t pos , const wchar_t* output , LPD3DXFONT font , color clr = color( 255 , 255 , 255 , 255 ) );
}