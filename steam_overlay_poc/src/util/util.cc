#include "util.hh"
#include <Windows.h>
#include <iostream>
#include <Psapi.h>

namespace util
{
	namespace console
	{
		FILE* stream = nullptr;

		void open( )
		{
			// allocate our console.
			AllocConsole( );

			// redirect stdout to our console.
			freopen_s( &stream, "CONOUT$", "w", stdout );
		}

		void log( const char* fmt, ... )
		{
			// print prefix.
			printf_s( "[+] " );

			va_list args;
			va_start( args, fmt );
			vprintf( fmt, args );
			va_end( args );

			// go to next line.
			printf_s( "\n" );
		}
	}

	namespace pattern
	{
		uintptr_t find( const char* module, std::vector<uint8_t> pattern )
		{
			// get module handle.
			HMODULE mod = GetModuleHandleA( module );

			// get module info.
			MODULEINFO info;
			GetModuleInformation( GetCurrentProcess( ), mod, &info, sizeof( MODULEINFO ) );

			// iterate through module memory.
			for ( uintptr_t i = uintptr_t( mod ); i < uintptr_t( mod ) + info.SizeOfImage; i++ )
			{
				// assume we found the pattern.
				bool found = true;

				// iterate through pattern.
				for ( size_t j = 0; j < pattern.size( ); j++ )
				{
					// skip mask bytes.
					if ( pattern[ j ] == 0xAE )
						continue;

					// compare bytes.
					if ( pattern[ j ] != *reinterpret_cast< uint8_t* >( i + j ) )
					{
						found = false;
						break;
					}
				}

				// found the pattern.
				if ( found )
					return i;
			}

			// failed to find the pattern.
			return 0x0;
		}
	}
}