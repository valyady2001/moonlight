#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "recv.h"

namespace netvar_manager
{
	typedef void( *NetvarHookCallback )( const c_recv_proxy_data* pData , void* pStruct , void* pOut );

	class CNetvarHookManager
	{
	public:

		void Hook( std::string table , std::string prop , NetvarHookCallback callback );
		void Initialize( );
		int GetOffset( const char* tableName , const char* propName );


	private:
		int Get_Prop( const char* tableName , const char* propName , recv_prop** prop = 0 );
		int Get_Prop( recv_table* recvTable , const char* propName , recv_prop** prop = 0 );
		recv_table* GetTable( const char* tableName );
		std::vector < recv_table* > m_tables;




	private:
	}; extern CNetvarHookManager netvar_hook_manager;
}

#define offset(type, var, offset) \
	type& var() { \
		return *(type*)(uintptr_t(this) + offset); \
	} \

#define foffset( type, ptr, offset ) ( *( type* ) ( ( std::uintptr_t ) (ptr) + ( offset ) ) )

#define netvar( t, func, table, prop ) \
t& func( ) { \
	static auto off = netvar_manager::netvar_hook_manager.GetOffset(table , prop); \
	return *( t* ) ( std::uintptr_t( this ) + off ); \
}

