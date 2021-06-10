#include "utilities.h"

#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))


uint64_t utilities::find_sig( const char* szModule , const char* szSignature )
{
	MODULEINFO modInfo;
	GetModuleInformation( GetCurrentProcess( ) , GetModuleHandleA( szModule ) , &modInfo , sizeof( MODULEINFO ) );
	DWORD startAddress = ( DWORD ) modInfo.lpBaseOfDll;
	DWORD endAddress = startAddress + modInfo.SizeOfImage;
	const char* pat = szSignature;
	DWORD firstMatch = 0;
	for ( DWORD pCur = startAddress; pCur < endAddress; pCur++ ) {
		if ( !*pat ) return firstMatch;
		if ( *( PBYTE ) pat == '\?' || *( BYTE* ) pCur == getByte( pat ) ) {
			if ( !firstMatch ) firstMatch = pCur;
			if ( !pat[ 2 ] ) return firstMatch;
			if ( *( PWORD ) pat == '\?\?' || *( PBYTE ) pat != '\?' ) pat += 3;
			else pat += 2;    //one ?
		}
		else {
			pat = szSignature;
			firstMatch = 0;
		}
	}
	return NULL;
}

bool utilities::goes_through_smoke(vec3_t start, vec3_t end) {
	typedef bool(__cdecl* goes_through_smoke)(vec3_t, vec3_t);
	static uint32_t goes_through_smoke_offset = (uint32_t)utilities::find_sig("client.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
	static goes_through_smoke goes_through_smoke_fn = (goes_through_smoke)goes_through_smoke_offset;
	return goes_through_smoke_fn(start, end);
}