#include "vcruntime.hpp"
#include <intrin.h>

namespace vcruntime {
    void* memcpy( void* dst, const void* src, size_t count ) {
        __movsb( reinterpret_cast< uint8_t* >( dst ), reinterpret_cast< const uint8_t* >( src ), count );
        return dst;
    }

    void* memset( void* src, int val, size_t count ) {
        __stosb( reinterpret_cast< uint8_t* >( src ), val, count );
        return src;
    }

    int memcmp( void* dst, void* src, size_t size ) {
        if ( size > 0 ) {
            const uint8_t* first = reinterpret_cast< const uint8_t* >( dst );
            const uint8_t* second = reinterpret_cast< const uint8_t* >( src );

            if ( first && second ) {
                do {
                    if ( *first++ != *second++ )
                        return ( *--first - *--second );
                } while ( --size != 0 );
            }
        }

        return 0;
    }

    void* memchr( void* dst, int chr, size_t size ) {
        if ( size > 0 ) {
            auto data = reinterpret_cast< const char* >( dst );

            if ( data ) {
                do {
                    if ( *data++ == chr )
                        return ( void* )( data - 1 );
                } while ( --size != 0 );
            }
        }

        return 0;
    }

    size_t strlen( const char* const str ) {
        size_t length{ };

        while ( str[ length ] != '\0' )
            ++length;

        return length;
    }

    size_t wcslen( const wchar_t* const str ) {
        size_t length{ };

        while ( str[ length ] != L'\0' )
            ++length;

        return length;
    }

    size_t strnlen( const char* const str, size_t size ) {
        for ( size_t idx = 0; idx < size; ++idx ) {
            if ( str[ idx ] == '\0' )
                return idx;
        }

        return size;
    }

    size_t wcsnlen( const wchar_t* const str, size_t size ) {
        for ( size_t idx = 0; idx < size; ++idx ) {
            if ( str[ idx ] == L'\0' )
                return idx;
        }

        return size;
    }

    char* strcpy( char* dst, const char* const src ) {
        for ( size_t idx = 0; src[ idx ] != '\0'; ++idx )
            dst[ idx ] = src[ idx ];

        return dst;
    }

    wchar_t* wcscpy( wchar_t* dst, const wchar_t* const src ) {
        for ( size_t idx = 0; src[ idx ] != L'\0'; ++idx )
            dst[ idx ] = src[ idx ];

        return dst;
    }

    char* strncpy( char* dst, const char* const src, size_t size ) {
        for ( size_t idx = 0; idx < size; ++idx ) {
            if ( src[ idx ] == '\0' )
                break;

            dst[ idx ] = src[ idx ];
        }

        return dst;
    }

    wchar_t* wcsncpy( wchar_t* dst, const wchar_t* const src, size_t size ) {
        for ( size_t idx{ }; idx < size; ++idx ) {
            if ( src[ idx ] == L'\0' )
                break;

            dst[ idx ] = src[ idx ];
        }

        return dst;
    }

    int strcmp( const char* const left, const char* const right ) {
        size_t idx{ };

        while ( left[ idx ] == right[ idx ] ) {
            if ( left[ idx ] == '\0' )
                return 0;

            ++idx;
        }

        return ( left[ idx ] - right[ idx ] );
    }

    int wcscmp( const wchar_t* const left, const wchar_t* const right ) {
        size_t idx{ };

        while ( left[ idx ] == right[ idx ] ) {
            if ( left[ idx ] == L'\0' )
                return 0;

            ++idx;
        }

        return ( left[ idx ] - right[ idx ] );
    }

    int strncmp( const char* left, const char* right, size_t size ) {
        int8_t result{ };

        while ( --size ) {
            if ( ( result = ( left[ 0 ] - right[ 0 ] ) ) != 0 || !*left++ || !*right++ )
                break;
        }

        return result;
    }

    int wcsncmp( const wchar_t* left, const wchar_t* right, size_t size ) {
        int8_t result{ };

        while ( --size ) {
            if ( ( result = ( left[ 0 ] - right[ 0 ] ) ) != 0 || !*left++ || !*right++ )
                break;
        }

        return result;
    }

    char tolower( char chr ) {
        if ( chr >= 'A' && chr <= 'Z' )
            return ( chr - ( 'A' - 'a' ) );

        return chr;
    }

    wchar_t towlower( wchar_t chr ) {
        if ( chr >= L'A' && chr <= L'Z' )
            return ( chr - ( L'A' - L'a' ) );

        return chr;
    }

    char toupper( char chr ) {
        if ( chr >= 'a' && chr <= 'z' )
            return ( chr - ( 'a' - 'A' ) );

        return chr;
    }

    wchar_t towupper( wchar_t chr ) {
        if ( chr >= L'a' && chr <= L'z' )
            return ( chr - ( L'a' - L'A' ) );

        return chr;
    }

    const wchar_t* wcschr( const wchar_t* str, wchar_t chr ) {
        const wchar_t* current = nullptr;

        for ( current = str; *current; current++ ) {
            if ( *current == chr )
                return current;
        }

        return nullptr;
    }

    const char* strchr( const char* str, int chr ) {
        const char* current = nullptr;

        for ( current = str; *current; current++ ) {
            if ( *current == chr )
                return ( char* )( current );
        }

        return nullptr;
    }

    const char* strstr( const char* left, const char* right ) {
        auto size = strlen( right );
        auto str = static_cast< char* >( nullptr );

        for ( str = ( char* )( left ); str != 0; str = const_cast< char* >( strchr( str, right[ 0 ] ) ) ) {
            if ( !strncmp( str, right, size ) )
                break;

            str++;
        }

        return str;
    }

    const wchar_t* wcsstr( const wchar_t* left, const wchar_t* right ) {
        auto size = wcslen( right );
        auto str = static_cast< wchar_t* >( nullptr );

        for ( str = ( wchar_t* )( left ); str != 0; str = const_cast< wchar_t* >( wcschr( str, right[ 0 ] ) ) ) {
            if ( !wcsncmp( str, right, size ) )
                break;

            str++;
        }

        return str;
    }

    int stricmp( const char* const left, const char* const right ) {
        size_t idx{ };

        while ( toupper( left[ idx ] ) == toupper( right[ idx ] ) ) {
            if ( left[ idx ] == '\0' )
                return 0;

            ++idx;
        }

        return ( toupper( left[ idx ] ) - toupper( right[ idx ] ) );
    }

    int wcsicmp( const wchar_t* const left, const wchar_t* const right ) {
        size_t idx{ };

        while ( towupper( left[ idx ] ) == towupper( right[ idx ] ) ) {
            if ( left[ idx ] == L'\0' )
                return 0;

            ++idx;
        }

        return ( towupper( left[ idx ] ) - towupper( right[ idx ] ) );
    }

    int strnicmp( const char* const left, const char* const right, size_t size ) {
        size_t idx{ };

        while ( toupper( left[ idx ] ) == toupper( right[ idx ] ) ) {
            if ( idx >= size )
                break;

            if ( left[ idx ] == '\0' )
                return 0;

            ++idx;
        }

        return ( toupper( left[ idx ] ) - toupper( right[ idx ] ) );
    }

    int wcsnicmp( const wchar_t* const left, const wchar_t* const right, size_t size ) {
        size_t idx{ };

        while ( towupper( left[ idx ] ) == towupper( right[ idx ] ) ) {
            if ( idx >= size )
                break;

            if ( left[ idx ] == L'\0' )
                return 0;

            ++idx;
        }

        return ( towupper( left[ idx ] ) - towupper( right[ idx ] ) );
    }

    int isdigit( int chr ) {
        return chr >= ( int )'0' && chr <= ( int )'9';
    }

    int isspace( int chr ) {
        return chr == ( int )' ';
    }
}