#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace util
{
	namespace console
	{
		void open( );
		void close( );
		void log( const char* fmt, ... );
	}

	namespace pattern
	{
		uintptr_t find( const char* module, std::vector< uint8_t > pattern );
	}
}