#pragma once

#include "../../includes/includes.hpp"

namespace process {
	uintptr_t get_base_address( uint32_t );
	uintptr_t get_process_dtb( uint32_t );

	bool read( uint32_t, uintptr_t, void*, size_t );
	bool write( uint32_t, uintptr_t, void*, size_t );
}