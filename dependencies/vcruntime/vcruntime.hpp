#pragma once

#include "../../includes/includes.hpp"

namespace vcruntime {
    void* memcpy( void*, const void*, size_t );
    void* memset( void*, int, size_t );

    int memcmp( void*, void*, size_t );
    void* memchr( void*, int, size_t );

    size_t strlen( const char* const );
    size_t wcslen( const wchar_t* const );

    size_t strnlen( const char* const, size_t );
    size_t wcsnlen( const wchar_t* const, size_t );

    char* strcpy( char*, const char* const );
    wchar_t* wcscpy( wchar_t*, const wchar_t* const );

    char* strncpy( char*, const char* const, size_t );
    wchar_t* wcsncpy( wchar_t*, const wchar_t* const, size_t );

    int strcmp( const char* const, const char* const );
    int wcscmp( const wchar_t* const, const wchar_t* const );

    int strncmp( const char*, const char*, size_t );
    int wcsncmp( const wchar_t*, const wchar_t*, size_t );

    char tolower( char );
    wchar_t towlower( wchar_t );

    char toupper( char );
    wchar_t towupper( wchar_t );

    const wchar_t* wcschr( const wchar_t*, wchar_t );
    const char* strchr( const char*, int );

    const char* strstr( const char*, const char* );
    const wchar_t* wcsstr( const wchar_t*, const wchar_t* );

    int stricmp( const char* const, const char* const );
    int wcsicmp( const wchar_t* const, const wchar_t* const );

    int strnicmp( const char* const, const char* const, size_t );
    int wcsnicmp( const wchar_t* const, const wchar_t* const, size_t );

    int isdigit( int );
    int isspace( int );
}