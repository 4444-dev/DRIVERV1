#include "winapi.hpp"

#include "../vcruntime/vcruntime.hpp"
#include "../importer/importer.hpp"
#include "../pattern/pattern.hpp"

namespace winapi {
	uintptr_t allocate_pool( size_t size, POOL_TYPE type ) {
		const auto pool = call( ExAllocatePoolWithTag, NonPagedPoolNx, size, 'Tsys' );

		if ( !pool )
			return 0;

		return reinterpret_cast< uintptr_t >( pool );
	}

	bool free_pool( uintptr_t address ) {
		if ( !address )
			return false;

		call( ExFreePoolWithTag, reinterpret_cast< void* >( address ), 'Tsys' );
		return true;
	}

	void unicode_to_ansi_string( PANSI_STRING dst, PCUNICODE_STRING src, BOOLEAN allocate_buffer ) {
		call( RtlUnicodeStringToAnsiString, dst, src, allocate_buffer );
	}

	void init_unicode_string( PUNICODE_STRING dst, PCWSTR src ) {
		call( RtlInitUnicodeString, dst, src );
	}

	void init_ansi_string( PANSI_STRING dst, PCSZ src ) {
		call( RtlInitAnsiString, dst, src );
	}

	void free_unicode_string( PUNICODE_STRING unicode_string ) {
		call( RtlFreeUnicodeString, unicode_string );
	}

	void free_ansi_string( PANSI_STRING ansi_string ) {
		call( RtlFreeAnsiString, ansi_string );
	}

	void sleep( uint32_t ms ) {
		auto interval = __int64( ms * -10000i64 );
		call( KeDelayExecutionThread, KernelMode, FALSE, reinterpret_cast< PLARGE_INTEGER >( &interval ) );
	}
}