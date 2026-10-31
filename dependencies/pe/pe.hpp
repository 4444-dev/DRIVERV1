#pragma once

#include "../../includes/includes.hpp"

namespace pe {
	typedef struct _section_t {
		uintptr_t m_base;
		uintptr_t m_size;
	} section_t, * psection_t;

	__forceinline section_t get_section( uintptr_t address, const char* name, size_t size ) {
		section_t section{ };

		auto nt_headers = reinterpret_cast< IMAGE_NT_HEADERS64* >( address + reinterpret_cast< IMAGE_DOS_HEADER* >( address )->e_lfanew );

		if ( nt_headers->Signature != 0x4550 )
			return { };

		auto sec = IMAGE_FIRST_SECTION( nt_headers );

		for ( uint16_t idx{ }; idx < nt_headers->FileHeader.NumberOfSections; ++idx, ++sec ) {
			if ( !strncmp( reinterpret_cast< const char* >( sec->Name ), name, size ) ) {
				section.m_base = address + sec->VirtualAddress;
				section.m_size = sec->Misc.VirtualSize;

				return section;
			}
		}

		return { };
	}
}