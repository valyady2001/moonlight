#include "vars.h"
#include <Windows.h>
#include <Psapi.h>
#include <sstream>
#include <fstream>
#include <string>

bool vars::save( std::string name )
{
	CreateDirectory( "C:\\moonlight" , NULL );
	std::string file_path = "C:\\moonlight\\" + name + ".cfg";

	std::fstream file( file_path , std::ios::out | std::ios::in | std::ios::trunc );
	file.close( );

	file.open( file_path , std::ios::out | std::ios::in );
	if ( !file.is_open( ) )
	{
		file.close( );
		return false;
	}

	const size_t settings_size = sizeof( vars );
	for ( int i = 0; i < settings_size; i++ )
	{
		byte current_byte = *reinterpret_cast< byte* >( uintptr_t( this ) + i );
		for ( int x = 0; x < 8; x++ )
		{
			file << ( int ) ( ( current_byte >> x ) & 1 );
		}
	}

	file.close( );

	return true;
}

bool vars::load( std::string file_name )
{
	CreateDirectory( "C:\\moonlight" , NULL );

	std::string file_path = "C:\\moonlight\\" + file_name + ".cfg";

	std::fstream file;
	file.open( file_path , std::ios::out | std::ios::in );
	if ( !file.is_open( ) )
	{
		file.close( );
		return false;
	}

	std::string line;
	while ( file )
	{
		std::getline( file , line );

		const size_t settings_size = sizeof( vars );
		if ( line.size( ) > settings_size * 8 )
		{
			file.close( );
			return false;
		}
		for ( int i = 0; i < settings_size; i++ )
		{
			byte current_byte = *reinterpret_cast< byte* >( uintptr_t( this ) + i );
			for ( int x = 0; x < 8; x++ )
			{
				if ( line[ ( i * 8 ) + x ] == '1' )
					current_byte |= 1 << x;
				else
					current_byte &= ~( 1 << x );
			}
			*reinterpret_cast< byte* >( uintptr_t( this ) + i ) = current_byte;
		}
	}

	file.close( );

	return true;
}

void vars::create(std::string name)
{
	std::string file_path = "C:\\moonlight\\" + name + ".cfg";

	std::fstream file;
	file.open( file_path , std::ios::out | std::ios::in | std::ios::trunc );
	file.close( );
}

std::vector<std::string> vars::getcfg( )
{
	std::vector<std::string> configs;

	WIN32_FIND_DATA ffd;
	auto directory = "C:\\moonlight\\*";
	auto hFind = FindFirstFile( directory , &ffd );

	while ( FindNextFile( hFind , &ffd ) )
	{
		if ( !( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			std::string file_name = ffd.cFileName;
			if ( file_name.size( ) < 4 ) // .cfg
				continue;

			std::string end = file_name;
			end.erase( end.begin( ) , end.end( ) - 4 ); // erase everything but the last 4 letters
			if ( end != ".cfg" )
				continue;

			file_name.erase( file_name.end( ) - 4 , file_name.end( ) ); // erase the .cfg part
			configs.push_back( file_name );
		}
	}
	return configs;
}

vars g_var;