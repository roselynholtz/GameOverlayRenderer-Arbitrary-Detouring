#include "util/util.hh"
#include <Windows.h>

// original console echo function.
int original_echo = 0;

static int detoured_echo( int* a1 )
{
	// notify the user this function was called.
	util::console::log( "echo callback called" );

	// call original function.
	return reinterpret_cast< int( __cdecl* )( int* ) >( original_echo )( a1 );
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
    // disable thread attach / detach notifications.
    DisableThreadLibraryCalls( hinstDLL );

    if ( fdwReason == DLL_PROCESS_ATTACH )
    {
        // open our console.
        util::console::open( );

        // print welcome message.
        util::console::log( "steam arbitrary hook poc - " __DATE__ );

        // store our hooking function.
        auto hook_func_wrapper = reinterpret_cast< char( __cdecl* )( unsigned __int8*, int, LPCVOID*, int ) >( util::pattern::find( "GameOverlayRenderer.dll", { 0x55, 0x8B, 0xEC, 0x51, 0x8B, 0x45, 0xAE, 0xC7 } ) );

        // we failed to find the hook func wrapper, don't bother.
        if ( !hook_func_wrapper )
        {
            // inform the user.
            util::console::log( "failed to find hook function" );

            // notify dll failed to load.
            return FALSE;
        }

        util::console::log( "hook_func_wrapper: 0x%p", hook_func_wrapper );

        // store console's echo command callback address.
        uintptr_t console_echo = util::pattern::find( "engine.dll", { 0x55, 0x8B, 0xEC, 0x8B, 0x0D, 0xAE, 0xAE, 0xAE, 0xAE, 0x53, 0x8B, 0x5D, 0x08 } );
        
        // we failed to find the console echo function, don't bother.
        if ( !console_echo )
		{
			// inform the user.
			util::console::log( "failed to find console echo function" );

			// notify dll failed to load.
			return FALSE;
		}

        // hook the console echo function.
        hook_func_wrapper( reinterpret_cast< unsigned __int8* >( console_echo ), reinterpret_cast< int >( detoured_echo ), reinterpret_cast< LPCVOID* >( &original_echo ), 0 );

		// inform the user that we've hooked the console echo function.
		util::console::log( "hooked console echo function (n: 0x%p | o: 0x%p)", detoured_echo, original_echo );

		// notify dll loaded successfully.
		return TRUE;
    }

    return TRUE;
}