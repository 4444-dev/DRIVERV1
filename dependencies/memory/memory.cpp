#include "memory.hpp"

#include "../importer/importer.hpp"
#include "../../globals/globals.hpp"

extern "C" void trampoline_stub( );
extern "C" uintptr_t trampoline_target;

namespace memory {
	bool is_valid( const uintptr_t address ) {
		if ( !address )
			return false;

		const auto result = call( MmIsAddressValid, reinterpret_cast< void* >( address ) );

		if ( !result )
			return false;

		return true;
	}

	bool is_valid( const void* address ) {
		if ( !address )
			return false;

		const auto result = call( MmIsAddressValid, const_cast< void* >( address ) );

		if ( !result )
			return false;

		return true;
	}

	bool hook( const wchar_t* name, uintptr_t function, PDRIVER_DISPATCH& original ) {
		const auto type = take( IoDriverObjectType );

		if ( !type )
			return false;

		UNICODE_STRING object_name{ };
		winapi::init_unicode_string( &object_name, name );

		PDRIVER_OBJECT driver_object{ };
		call( ObReferenceObjectByName, &object_name, OBJ_CASE_INSENSITIVE, 0, 0, *type, KernelMode, 0, &driver_object );

		if ( !driver_object )
			return false;

		original = driver_object->MajorFunction[ 14 ];

		trampoline_target = function;
		driver_object->HardwareDatabase = reinterpret_cast< PUNICODE_STRING >( &trampoline_stub );

		InterlockedExchange64( reinterpret_cast< volatile __int64* >( &driver_object->MajorFunction[ 14 ] ), globals::trampoline );

		return true;
	}
}