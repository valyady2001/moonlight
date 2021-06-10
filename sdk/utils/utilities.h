#pragma once
#include <Windows.h>
#include <algorithm>
#include <vector>
#include <Psapi.h>
#include <sdk/math/vec3.h>

typedef void* ( *CreateInterfaceFn )        ( const char* pName , int* pReturnCode );
class base_entity;
namespace utilities
{
	uint64_t find_sig( const char* szModule , const char* szSignature );
	bool goes_through_smoke( vec3_t start , vec3_t end );
	inline void UpdateAllViewmodelAddons( base_entity* entity ) {
		using UpdateAllViewmodelAddons_t = int( __fastcall* )( base_entity* );
		static UpdateAllViewmodelAddons_t UpdateAllViewmodelAddonsFn = ( UpdateAllViewmodelAddons_t ) ( find_sig( "client.dll" , "55 8B EC 83 E4 ? 83 EC ? 53 8B D9 56 57 8B 03 FF 90 ? ? ? ? 8B F8 89 7C 24 ? 85 FF 0F 84 ? ? ? ? 8B 17 8B CF" ) );

		if ( !UpdateAllViewmodelAddonsFn )
			return;

		UpdateAllViewmodelAddonsFn( entity );
	}

};
