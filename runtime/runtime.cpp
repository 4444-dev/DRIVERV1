#include <cstdint>

#pragma comment( linker, "/merge:.pdata=.rdata" )
#pragma comment( linker, "/merge:.rdata=.data" )

extern "C" {
    int32_t _fltused = 0x9875;
};

extern "C" {
    void __security_check_cookie( uintptr_t cookie ) { }
    void __chkstk( ) { }
    void __GSHandlerCheck( ) { }

    void __cdecl __std_terminate( ) {
        __debugbreak( );
    }
}