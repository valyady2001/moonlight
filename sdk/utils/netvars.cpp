#include "netvars.h"
#include <fstream>
#include <utility>
#include <interfaces/interfaces.h>


namespace netvar_manager
{
	CNetvarHookManager netvar_hook_manager;
	void CNetvarHookManager::Hook( std::string table_to_hook , std::string prop_to_hook , NetvarHookCallback callback )
	{
		auto client_class = interfaces::client->get_all_classes( );

		// go through all classes
		while ( client_class )
		{
			std::string table_name = client_class->recv_table->table_name;

			if ( table_name == table_to_hook )
			{
				for ( int i = 0; i < client_class->recv_table->props_count; i++ )
				{
					auto& prop = client_class->recv_table->props[ i ];
					std::string prop_name = prop.prop_name;

					if ( prop_name == prop_to_hook )
					{
						prop.proxy_fn = callback;
					}
				}
			}

			client_class = client_class->next; // go to the next class
		}
	}
	
	int CNetvarHookManager::GetOffset( const char* tableName , const char* propName )
	{
		int offset = Get_Prop( tableName , propName );
		if ( !offset )
		{
			//Msg ( "%s not found!\n", propName );
			return 0;
		}
		//Msg ( "%s: 0x%02X\n", propName, offset );
		return offset;
	}
	int CNetvarHookManager::Get_Prop( const char* tableName , const char* propName , recv_prop** prop )
	{
		recv_table* recvTable = GetTable( tableName );
		if ( !recvTable )
			return 0;

		int offset = Get_Prop( recvTable , propName , prop );
		if ( !offset )
			return 0;

		return offset;
	}
	void CNetvarHookManager::Initialize( )
	{
		m_tables.clear( );

		client_class_t* clientClass = interfaces::client->get_all_classes( );
		if ( !clientClass )
			return;

		while ( clientClass )
		{
			recv_table* recvTable = clientClass->recv_table;
			m_tables.push_back( recvTable );

			clientClass = clientClass->next;
		}
	}
	recv_table* CNetvarHookManager::GetTable( const char* tableName )
	{
		if ( m_tables.empty( ) )
			return 0;

		for  (auto *table : m_tables )
		{
			if ( !table )
				continue;

			if ( _stricmp( table->table_name , tableName ) == 0 )
				return table;
		}

		return 0;
	}
	int CNetvarHookManager::Get_Prop( recv_table* recvTable , const char* propName , recv_prop** prop )
	{
		int extraOffset = 0;
		for ( int i = 0; i < recvTable->props_count; ++i )
		{
			recv_prop* recvProp = &recvTable->props[ i ];
			recv_table* child = recvProp->data_table;

			if ( child && ( child->props_count > 0 ) )
			{
				int tmp = Get_Prop( child , propName , prop );
				if ( tmp )
					extraOffset += ( recvProp->offset + tmp );
			}

			if ( _stricmp( ( const char* ) recvProp->prop_name , propName ) )
				continue;

			if ( prop )
				*prop = recvProp;

			return ( recvProp->offset + extraOffset );
		}

		return extraOffset;
	}
}
