#pragma once

#include "../../includes/includes.hpp"

#include "../definitions/definitions.hpp"
#include "../vcruntime/vcruntime.hpp"
#include "../importer/importer.hpp"

namespace physical {
	bool read_phys( uintptr_t, void*, size_t, size_t* );
	bool write_phys( uintptr_t, void*, size_t, size_t* );

	uintptr_t virt_to_phys( uintptr_t, uintptr_t );
	uintptr_t get_pte_phys( uintptr_t, uintptr_t, bool* = nullptr );
}