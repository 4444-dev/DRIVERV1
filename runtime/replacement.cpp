#include "../dependencies/vcruntime/vcruntime.hpp"
#include "../dependencies/importer/importer.hpp"
#include "../dependencies/winapi/winapi.hpp"

#pragma function( memset )
void* memset( void* dst, int src, size_t size ) {
    return vcruntime::memset( dst, src, size );
}

#pragma function( memcpy )
void* memcpy( void* dst, const void* src, size_t size ) {
    return vcruntime::memcpy( dst, src, size );
}

#pragma function( memcmp )
int memcmp( const void* dst, const void* src, size_t size ) {
    return vcruntime::memcmp( const_cast< void* >( dst ), const_cast< void* >( src ), size );
}

#pragma function( memmove )
void* memmove( void* dst, const void* src, size_t count ) {
    return vcruntime::memcpy( dst, src, count );
}

#pragma function( memchr )
const void* memchr( const void* dst, int chr, size_t size ) {
    return vcruntime::memchr( const_cast< void* >( dst ), chr, size );
}

#pragma function( strlen )
size_t strlen( const char* const str ) {
    return vcruntime::strlen( str );
}

#pragma function( wcslen )
size_t wcslen( const wchar_t* const str ) {
    return vcruntime::wcslen( str );
}

#pragma function( strnlen )
size_t strnlen( const char* const str, size_t size ) {
    return vcruntime::strnlen( str, size );
}

#pragma function( wcsnlen )
size_t wcsnlen( const wchar_t* const str, size_t size ) {
    return vcruntime::wcsnlen( str, size );
}

#pragma function( strcpy )
char* strcpy( char* dst, const char* const src ) {
    return vcruntime::strcpy( dst, src );
}

#pragma function( wcscpy )
wchar_t* wcscpy( wchar_t* dst, const wchar_t* const src ) {
    return vcruntime::wcscpy( dst, src );
}

#pragma function( strncpy )
char* strncpy( char* dst, const char* const src, size_t size ) {
    return vcruntime::strncpy( dst, src, size );
}

#pragma function( wcsncpy )
wchar_t* wcsncpy( wchar_t* dst, const wchar_t* const src, size_t size ) {
    return vcruntime::wcsncpy( dst, src, size );
}

#pragma function( strcmp )
int strcmp( const char* const left, const char* const right ) {
    return vcruntime::strcmp( left, right );
}

#pragma function( wcscmp )
int wcscmp( const wchar_t* const left, const wchar_t* const right ) {
    return vcruntime::wcscmp( left, right );
}

#pragma function( strncmp )
int strncmp( const char* left, const char* right, size_t size ) {
    return vcruntime::strncmp( left, right, size );
}

#pragma function( wcsncmp )
int wcsncmp( const wchar_t* left, const wchar_t* right, size_t size ) {
    return vcruntime::wcsncmp( left, right, size );
}

#pragma function( tolower )
char tolower( char chr ) {
    return vcruntime::tolower( chr );
}

#pragma function( towlower )
wchar_t towlower( wchar_t chr ) {
    return vcruntime::towlower( chr );
}

#pragma function( toupper )
char toupper( char chr ) {
    return vcruntime::toupper( chr );
}

#pragma function( towupper )
wchar_t towupper( wchar_t chr ) {
    return vcruntime::towupper( chr );
}

#pragma function( wcschr )
const wchar_t* wcschr( const wchar_t* str, wchar_t chr ) {
    return vcruntime::wcschr( str, chr );
}

#pragma function( strchr )
const char* strchr( const char* str, int chr ) {
    return vcruntime::strchr( str, chr );
}

#pragma function( strstr )
const char* strstr( const char* left, const char* right ) {
    return vcruntime::strstr( left, right );
}

#pragma function( wcsstr )
const wchar_t* wcsstr( const wchar_t* left, const wchar_t* right ) {
    return vcruntime::wcsstr( left, right );
}

#pragma function( stricmp )
int stricmp( const char* const left, const char* const right ) {
    return vcruntime::stricmp( left, right );
}

#pragma function( wcsicmp )
int wcsicmp( const wchar_t* const left, const wchar_t* const right ) {
    return vcruntime::wcsicmp( left, right );
}

#pragma function( strnicmp )
int strnicmp( const char* const left, const char* const right, size_t size ) {
    return vcruntime::strnicmp( left, right, size );
}

#pragma function( wcsnicmp )
int wcsnicmp( const wchar_t* const left, const wchar_t* const right, size_t size ) {
    return vcruntime::wcsnicmp( left, right, size );
}

#pragma function( isdigit )
int isdigit( int chr ) {
    return vcruntime::isdigit( chr );
}

#pragma function( isspace )
int isspace( int chr ) {
    return vcruntime::isspace( chr );
}

void* calloc( size_t count, size_t size ) {
    return reinterpret_cast< void* >( winapi::allocate_pool( size * count ) );
}

void* malloc( size_t size ) {
    return reinterpret_cast< void* >( winapi::allocate_pool( size ) );
}

void free( void* pointer ) {
    winapi::free_pool( reinterpret_cast< uintptr_t >( pointer ) );
}

void* operator new( size_t size ) {
    return reinterpret_cast< void* >( winapi::allocate_pool( size ) );
}

void* operator new[ ]( size_t size ) {
    return reinterpret_cast< void* >( winapi::allocate_pool( size ) );
}

void operator delete( void* data ) noexcept {
    winapi::free_pool( reinterpret_cast< uintptr_t >( data ) );
}

void operator delete( void* data, size_t size ) noexcept {
    winapi::free_pool( reinterpret_cast< uintptr_t >( data ) );
}

void operator delete[ ]( void* data ) noexcept {
    winapi::free_pool( reinterpret_cast< uintptr_t >( data ) );
}

void operator delete[ ]( void* data, size_t size ) noexcept {
    winapi::free_pool( reinterpret_cast< uintptr_t >( data ) );
}