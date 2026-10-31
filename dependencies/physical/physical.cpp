#include "physical.hpp"

#include "../vcruntime/vcruntime.hpp"
#include "../importer/importer.hpp"

namespace physical {
	bool read_phys( uintptr_t address, void* buffer, size_t size, size_t* read ) {
		MM_COPY_ADDRESS copy{ };
		copy.PhysicalAddress.QuadPart = static_cast< int64_t >( address );

		const auto status = call( MmCopyMemory, buffer, copy, size, MM_COPY_MEMORY_PHYSICAL, read );

		if ( !NT_SUCCESS( status ) )
			return false;

		return true;
	}

	bool write_phys( uintptr_t address, void* buffer, size_t size, size_t* written ) {
		if ( !address )
			return false;

		PHYSICAL_ADDRESS phys_address{ };
		phys_address.QuadPart = static_cast< int64_t >( address );

		const auto mapped = call( MmMapIoSpaceEx, phys_address, size, PAGE_READWRITE );

		if ( !mapped )
			return false;

		memcpy( mapped, buffer, size );

		if ( written )
			*written = size;

		call( MmUnmapIoSpace, mapped, size );
		return true;
	}

	uintptr_t virt_to_phys( uintptr_t dtb, uintptr_t address ) {
		static uintptr_t MASK{ };

		if ( !MASK )
			MASK = ~0xfull << 8 & 0xfffffffffull;

		dtb &= ~0xf;

		const auto offset = address & ~( ~0ul << 12 );
		const auto pte = ( ( address >> 12 ) & 0x1ffll );
		const auto pdp = ( ( address >> 39 ) & 0x1ffll );
		const auto pt = ( ( address >> 21 ) & 0x1ffll );
		const auto pd = ( ( address >> 30 ) & 0x1ffll );

		size_t read{ };
		uintptr_t pdpe{ };

		read_phys( ( dtb + 8 * pdp ), &pdpe, sizeof( pdpe ), &read );

		if ( ~pdpe & 1 )
			return 0;

		uintptr_t pde{ };
		read_phys( ( ( pdpe & MASK ) + 8 * pd ), &pde, sizeof( pde ), &read );

		if ( ~pde & 1 )
			return 0;

		if ( pde & 0x80 )
			return ( pde & ~0ull << 42 >> 12 ) + ( address & ~( ~0ull << 30 ) );

		uintptr_t ppte{ };
		read_phys( ( ( pde & MASK ) + 8 * pt ), &ppte, sizeof( ppte ), &read );

		if ( ~ppte & 1 )
			return 0;

		if ( ppte & 0x80 )
			return ( ppte & MASK ) + ( address & ~( ~0ull << 21 ) );

		address = 0;
		read_phys( ( ( ppte & MASK ) + 8 * pte ), &address, sizeof( address ), &read );
		address &= MASK;

		if ( !address )
			return 0;

		return address + offset;
	}

	uintptr_t get_pte_phys( uintptr_t dtb, uintptr_t address, bool* is_large_page ) {
		static uintptr_t MASK{ };

		if ( !MASK )
			MASK = ~0xfull << 8 & 0xfffffffffull;

		dtb &= ~0xf;

		const auto pte = ( ( address >> 12 ) & 0x1ffll );
		const auto pdp = ( ( address >> 39 ) & 0x1ffll );
		const auto pt = ( ( address >> 21 ) & 0x1ffll );
		const auto pd = ( ( address >> 30 ) & 0x1ffll );

		size_t read{ };
		uintptr_t pdpe{ };

		read_phys( ( dtb + 8 * pdp ), &pdpe, sizeof( pdpe ), &read );

		if ( ~pdpe & 1 )
			return 0;

		uintptr_t pde{ };
		read_phys( ( ( pdpe & MASK ) + 8 * pd ), &pde, sizeof( pde ), &read );

		if ( ~pde & 1 )
			return 0;

		if ( pde & 0x80 ) {
			if ( is_large_page )
				*is_large_page = true;
			return ( pdpe & MASK ) + 8 * pd;
		}

		uintptr_t ppte{ };
		read_phys( ( ( pde & MASK ) + 8 * pt ), &ppte, sizeof( ppte ), &read );

		if ( ~ppte & 1 )
			return 0;

		if ( ppte & 0x80 ) {
			if ( is_large_page )
				*is_large_page = true;
			return ( pde & MASK ) + 8 * pt;
		}

		if ( is_large_page )
			*is_large_page = false;

		return ( ppte & MASK ) + 8 * pte;
	}
}