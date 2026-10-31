#pragma once

#include "../../includes/includes.hpp"

namespace memory {
	bool is_valid( const uintptr_t );
	bool is_valid( const void* );

	bool hook( const wchar_t*, uintptr_t, PDRIVER_DISPATCH& );

	template< typename _Ty >
	__forceinline void write( const uintptr_t address, _Ty value ) {
		*reinterpret_cast< _Ty* >( address ) = value;
	}

	template< typename _Ty >
	__forceinline _Ty read( const uintptr_t address ) {
		return *reinterpret_cast< _Ty* >( address );
	}
}