#include "render.h"
#include "fonts.h"

bool render::initialize( IDirect3DDevice9* device ) {
	if ( !device )
		return false;

	render::device = device;

	void* visitor_dataa = ( void* ) ( visitor_data );
	DWORD visitor = 0 , visitor_len = visitor_length;
	AddFontMemResourceEx( visitor_dataa , visitor_len , nullptr , &visitor );

	D3DXCreateFont( render::device , 14 , 0 , FW_NORMAL , 0 , false , DEFAULT_CHARSET , OUT_DEFAULT_PRECIS , CLEARTYPE_QUALITY , DEFAULT_PITCH , "Segoe UI" , &fonts::segoeui );
	if ( !fonts::segoeui )
		return false;

	D3DXCreateFont( render::device , 18 , 0 , FW_MEDIUM , 0 , false , DEFAULT_CHARSET , OUT_DEFAULT_PRECIS , CLEARTYPE_QUALITY , DEFAULT_PITCH , "Segoe UI" , &fonts::segoeui_bigger );
	if ( !fonts::segoeui_bigger )
		return false;

	D3DXCreateFont( render::device , 10 , 0 , FW_NORMAL , 0 , false , DEFAULT_CHARSET , OUT_DEFAULT_PRECIS , ANTIALIASED_QUALITY , DEFAULT_PITCH , "Visitor TT2 (BRK)" , &fonts::tahoma_bold );
	if ( !fonts::tahoma_bold )
		return false;

	D3DXCreateFont( render::device , 12 , 0 , FW_NORMAL , 0 , false , DEFAULT_CHARSET , OUT_DEFAULT_PRECIS , CLEARTYPE_QUALITY , DEFAULT_PITCH , "Verdana" , &fonts::verdana_small );
	if ( !fonts::verdana_small )
		return false;

	D3DXCreateFont( render::device , 24 , 0 , FW_NORMAL , 0 , false , DEFAULT_CHARSET , OUT_DEFAULT_PRECIS , CLEARTYPE_QUALITY , DEFAULT_PITCH , "Verdana" , &fonts::unicode );
	if ( !fonts::unicode )
		return false;


	void* smallest_pixel_data = ( void* ) ( SmallestPixelRaw );
	DWORD smallest_pixel = 0 , smallest_pixel_len = SmallestPixelLength;
	AddFontMemResourceEx( smallest_pixel_data , smallest_pixel_len , nullptr , &smallest_pixel );

	D3DXCreateFont( render::device , 10 , 0 , FW_NORMAL , 0 , false , DEFAULT_CHARSET , OUT_DEFAULT_PRECIS , NONANTIALIASED_QUALITY , DEFAULT_PITCH , "Smallest Pixel-7" , &fonts::smallest_pixel );
	if ( !fonts::smallest_pixel )
		return false;

	return true;
}

void render::reset( ) {
	if ( fonts::segoeui )
		fonts::segoeui->Release( );

	if ( fonts::segoeui_bigger )
		fonts::segoeui_bigger->Release( );

	if ( fonts::tahoma_bold )
		fonts::tahoma_bold->Release( );

	if ( fonts::smallest_pixel )
		fonts::smallest_pixel->Release( );

	if ( fonts::verdana_small )
		fonts::verdana_small->Release( );

	if ( fonts::unicode )
		fonts::unicode->Release( );
}

void render::set_states( ) {
	D3DVIEWPORT9 scrn;
	device->GetViewport( &scrn );
	device->SetVertexShader( nullptr );
	device->SetPixelShader( nullptr );
	device->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
	device->SetRenderState( D3DRS_LIGHTING , false );
	device->SetRenderState( D3DRS_FOGENABLE , false );
	device->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );
	device->SetRenderState( D3DRS_FILLMODE , D3DFILL_SOLID );
	device->SetRenderState( D3DRS_ZENABLE , D3DZB_FALSE );
	device->SetRenderState( D3DRS_SCISSORTESTENABLE , true );
	device->SetRenderState( D3DRS_ZWRITEENABLE , false );
	device->SetRenderState( D3DRS_STENCILENABLE , false );
	device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , false );
	device->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE , false );
	device->SetRenderState( D3DRS_ALPHABLENDENABLE , true );
	device->SetRenderState( D3DRS_ALPHATESTENABLE , false );
	device->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE , true );
	device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
	device->SetRenderState( D3DRS_SRCBLENDALPHA , D3DBLEND_INVDESTALPHA );
	device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
	device->SetRenderState( D3DRS_DESTBLENDALPHA , D3DBLEND_ONE );
	device->SetRenderState( D3DRS_SRGBWRITEENABLE , false );
	device->SetRenderState( D3DRS_COLORWRITEENABLE , D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
}

D3DVIEWPORT9 render::get_viewport( )
{
	D3DVIEWPORT9 vp;
	device->GetViewport( &vp );
	return vp;
}

void render::set_viewport( D3DVIEWPORT9 vp )
{
	device->SetViewport( &vp );
}

D3DVIEWPORT9 render::start_clipping( vec2_t pos , vec2_t size )
{
	const auto old_vp = get_viewport( ) , new_vp = D3DVIEWPORT9{ unsigned long( pos.x ) , unsigned long( pos.y ) , unsigned long( size.x ) , unsigned long( size.y ) };
	set_viewport( new_vp );
	return old_vp;
}

void render::end_clipping( D3DVIEWPORT9 vp )
{
	set_viewport( vp );
}

void render::filled_rect( vec2_t pos , vec2_t size , color clr )
{
	vertex_t vtx[ 4 ] = { { pos.x, pos.y + size.y , 0.0f, 1.0f, color::to_d3d( clr ) }, { pos.x, pos.y, 0.0f, 1.0f, color::to_d3d( clr ) }, { pos.x + size.x, pos.y + size.y, 0.0f, 1.0f, color::to_d3d( clr ) }, { pos.x + size.x, pos.y, 0.0f, 1.0f, color::to_d3d( clr ) } };

	device->SetTexture( 0 , nullptr );
	device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , vtx , sizeof( vertex_t ) );
}

void render::filled_gradient_rect( vec2_t pos , vec2_t size , color clr1 , color clr2 , bool vertical )
{
	const auto one = color::to_d3d( clr1 ) , two = color::to_d3d( clr2 );
	std::uint32_t c1 , c2 , c3 , c4;

	if ( !vertical ) {
		c1 = one;
		c2 = two;
		c3 = one;
		c4 = two;
	}
	else {
		c1 = one;
		c2 = one;
		c3 = two;
		c4 = two;
	}

	vertex_t vert[ 4 ] = {
		{ pos.x, pos.y, 0.0f, 1.0f, c1 },
		{ pos.x + size.x, pos.y, 0.0f, 1.0f, c2 },
		{ pos.x, pos.y + size.y, 0.0f, 1.0f, c3 },
		{ pos.x + size.x, pos.y + size.y, 0.0f, 1.0f, c4 }
	};

	device->SetTexture( 0 , nullptr );
	device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP , 2 , &vert , sizeof( vertex_t ) );
}

void render::outline_rect( vec2_t pos , vec2_t size , color clr )
{
	/* stanga */
	line( pos , pos + vec2_t( 0 , size.y ) , clr );

	/* dreapta */
	line( pos + vec2_t( size.x - 1 , 0 ) , pos + vec2_t( size.x - 1 , size.y ) , clr );

	/* sus */
	line( pos + vec2_t( 1 , 0 ) , pos + vec2_t( size.x - 1 , 0 ) , clr );

	/* jos */
	line( pos + vec2_t( 1 , size.y - 1 ) , pos + vec2_t( size.x - 1 , size.y - 1 ) , clr );
}

void render::line( vec2_t start , vec2_t end , color clr )
{
	vertex_t vtx[ 2 ] = { { start.x, start.y, 0.0f, 1.0f, color::to_d3d( clr ) }, { end.x, end.y, 0.0f, 1.0f, color::to_d3d( clr ) } };

	device->SetTexture( 0 , nullptr );
	device->DrawPrimitiveUP( D3DPT_LINELIST , 1 , &vtx , sizeof( vertex_t ) );
}

vec2_t render::text_size( std::string_view txt , ID3DXFont* font )
{
	RECT rect = RECT( );
	font->DrawTextA( nullptr , txt.data( ) , txt.length( ) , &rect , DT_CALCRECT , D3DCOLOR_RGBA( 0 , 0 , 0 , 0 ) );

	vec2_t ret( rect.right - rect.left , rect.bottom - rect.top );
	return ret;
}

void render::text( vec2_t pos , std::string_view output , LPD3DXFONT font , color clr , int flags )
{
	RECT rect = RECT( );

	if ( flags != textflags::textflag_none ) {
		if ( flags & textflags::textflag_centeredx || flags & textflags::textflag_centeredy ) {
			vec2_t textsize = text_size( output , font );
			if ( flags & textflags::textflag_centeredx )
				pos.x -= textsize.x / 2;
			if ( flags & textflags::textflag_centeredy )
				pos.y -= textsize.y / 2;
		}

		if ( flags & textflags::textflag_outline ) {
			/* top left */
			SetRect( &rect , pos.x - 1 , pos.y - 1 , pos.x - 1 , pos.y - 1 );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 170 ) ) );

			/* top middle */
			SetRect( &rect , pos.x , pos.y - 1 , pos.x , pos.y - 1 );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 120 ) ) );

			/* top right */
			SetRect( &rect , pos.x + 1 , pos.y - 1 , pos.x + 1 , pos.y - 1 );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 120 ) ) );

			/* bottom left */
			SetRect( &rect , pos.x - 1 , pos.y + 1 , pos.x - 1 , pos.y + 1 );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 120 ) ) );

			/* bottom middle */
			SetRect( &rect , pos.x , pos.y + 1 , pos.x , pos.y + 1 );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 120 ) ) );

			/* bottom right */
			SetRect( &rect , pos.x + 1 , pos.y + 1 , pos.x + 1 , pos.y + 1 );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 120 ) ) );

			/* left */
			SetRect( &rect , pos.x - 1 , pos.y , pos.x - 1 , pos.y );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 120 ) ) );

			/* right */
			SetRect( &rect , pos.x + 1 , pos.y , pos.x + 1 , pos.y );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , 120 ) ) );
		}
		else if ( flags & textflags::textflag_dropshadow ) {
			/* bottom right */
			SetRect( &rect , pos.x + 1 , pos.y + 1 , pos.x + 1 , pos.y + 1 );
			font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( color( 0 , 0 , 0 , clr.a ) ) );
		}
	}

	SetRect( &rect , pos.x , pos.y , pos.x , pos.y );
	font->DrawTextA( nullptr , output.data( ) , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( clr ) );
}


void render::text_unicode( vec2_t pos , const wchar_t* output , LPD3DXFONT font , color clr )
{
	RECT rect = RECT( );
	SetRect( &rect , pos.x , pos.y , pos.x , pos.y );
	font->DrawTextW( nullptr , output , -1 , &rect , DT_LEFT | DT_NOCLIP , color::to_d3d( clr ) );
}