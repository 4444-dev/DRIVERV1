#pragma once

#include "../../includes/includes.hpp"

namespace winapi {
	bool free_pool( uintptr_t );

	void unicode_to_ansi_string( PANSI_STRING, PCUNICODE_STRING, BOOLEAN );

	void init_unicode_string( PUNICODE_STRING, PCWSTR );
	void init_ansi_string( PANSI_STRING, PCSZ );

	void free_unicode_string( PUNICODE_STRING );
	void free_ansi_string( PANSI_STRING );

	void sleep( uint32_t );

	uintptr_t allocate_independent_page( size_t );
	uintptr_t allocate_pool( size_t, POOL_TYPE = NonPagedPoolNx );
}