#include "pattern.hpp"
#include "../vcruntime/vcruntime.hpp"

#define in_range( x, a, b ) ( x >= a && x <= b ) 
#define get_bits( x ) ( in_range( ( x & ( ~0x20 ) ), 'A', 'F' ) ? ( ( x & ( ~0x20 ) ) - 'A' + 0xA ) : ( in_range( x, '0', '9' ) ? x - '0' : 0 ) )
#define get_byte( x ) ( get_bits( x[ 0 ] ) << 4 | get_bits( x[ 1 ] ) )

namespace pattern {
    uintptr_t find( uintptr_t base, size_t range, const char* pattern, const char* mask ) {
        const auto check_mask = [ ]( const char* base, const char* pattern, const char* mask ) -> bool {
            for ( ; *mask; ++base, ++pattern, ++mask ) {
                if ( *mask == 'x' && *base != *pattern ) {
                    return false;
                }
            }

            return true;
        };

        range = range - strlen( mask );

        for ( size_t idx{ 0 }; idx < range; ++idx ) {
            if ( check_mask( ( const char* )base + idx, pattern, mask ) ) {
                return base + idx;
            }
        }

        return 0;
    }

    uintptr_t find( uintptr_t base, const char* pattern, const char* mask ) {
        auto nt_header = ( IMAGE_NT_HEADERS* )( base + ( ( IMAGE_DOS_HEADER* )base )->e_lfanew );
        auto sections = IMAGE_FIRST_SECTION( nt_header );

        for ( size_t idx{ 0 }; idx < nt_header->FileHeader.NumberOfSections; idx++ ) {
            auto section = &sections[ idx ];

            if ( section->Characteristics & IMAGE_SCN_MEM_EXECUTE ) {
                const auto match = find( base + section->VirtualAddress, section->Misc.VirtualSize, pattern, mask );

                if ( match )
                    return match;
            }
        }

        return 0;
    }

    uintptr_t find( uintptr_t base, const char* pattern ) {
        auto pattern_ = pattern;
        uintptr_t first_match = 0;

        if ( !base )
            return 0;

        const auto nt = reinterpret_cast< IMAGE_NT_HEADERS* >( base + reinterpret_cast< IMAGE_DOS_HEADER* >( base )->e_lfanew );

        for ( auto current = base; current < base + nt->OptionalHeader.SizeOfImage; current++ ) {
            if ( !*pattern_ )
                return first_match;

            if ( *( uint8_t* )pattern_ == '\?' || *( uint8_t* )current == get_byte( pattern_ ) ) {
                if ( !first_match )
                    first_match = current;

                if ( !pattern_[ 2 ] )
                    return first_match;

                if ( *( uint16_t* )pattern_ == '\?\?' || *( uint8_t* )pattern_ != '\?' )
                    pattern_ += 3;

                else
                    pattern_ += 2;
            } else {
                pattern_ = pattern;
                first_match = 0;
            }
        }

        return 0;
    }

}