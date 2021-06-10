#pragma once
class base_entity;
namespace csgo
{
	void __cdecl rebuilt_cl_sendmove(  );
	void __cdecl rebuilt_cl_move( float accumulated_extra_samples , bool bFinalTick );
	inline decltype( &rebuilt_cl_move ) rb_clmove;
	inline decltype( &rebuilt_cl_sendmove ) rb_clsendmove;
	void SetupLean( base_entity* e );
}


