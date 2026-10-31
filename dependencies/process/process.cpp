#include "process.hpp"

#include "../definitions/definitions.hpp"
#include "../importer/importer.hpp"
#include "../physical/physical.hpp"

namespace process {
	uintptr_t get_base_address( uint32_t pid ) {
		PEPROCESS process{ };

		if ( !NT_SUCCESS( call( PsLookupProcessByProcessId, ( HANDLE )pid, &process ) ) )
			return 0;

		uintptr_t base_address = 0;

		const auto address = call( PsGetProcessSectionBaseAddress, process );
		if ( address ) {
			base_address = reinterpret_cast< uintptr_t >( address );
		} else {
			// Fallback: When anti-cheat (ACE / NetEase) zeroes SectionBaseAddress,
			// read PEB->ImageBaseAddress directly from the process context.
			const auto peb = call( PsGetProcessPeb, process );
			if ( peb ) {
				KAPC_STATE apc_state{ };
				call( KeStackAttachProcess, process, &apc_state );
				__try {
					base_address = *reinterpret_cast< uintptr_t* >( reinterpret_cast< uintptr_t >( peb ) + 0x10 );
				} __except ( 1 ) {
					base_address = 0;
				}
				call( KeUnstackDetachProcess, &apc_state );
			}
		}

		call( ObfDereferenceObject, process );
		return base_address;
	}

	uintptr_t get_process_dtb( uint32_t pid ) {
		static uint32_t cached_pid{ };
		static uintptr_t cached_dtb{ };

		if ( cached_pid == pid && cached_dtb )
			return cached_dtb;

		const auto address = get_base_address( pid );

		if ( !address )
			return 0;

		virt_addr_t virt_addr{ };

		virt_addr.m_value = address;

		const auto ranges = call( MmGetPhysicalMemoryRanges );

		if ( !memory::is_valid( ranges ) )
			return 0;

		for ( int i{ }; ; ++i ) {
			const auto element = &ranges[ i ];

			if ( !element->BaseAddress.QuadPart || !element->NumberOfBytes.QuadPart )
				break;

			auto phys_address = element->BaseAddress.QuadPart;

			for ( int j{ }; j < ( element->NumberOfBytes.QuadPart / 0x1000 ); j++, phys_address += 0x1000 ) {
				static uintptr_t mm_pfn_database{ };

				if ( !mm_pfn_database ) {
					const auto mm_get_virtual_for_physical = importer::find_export( HASH( "MmGetVirtualForPhysical" ) );

					if ( !memory::is_valid( mm_get_virtual_for_physical ) ) {
						call( ExFreePoolWithTag, ranges, 0 );
						return 0;
					}

					mm_pfn_database = reinterpret_cast< uintptr_t >( PAGE_ALIGN( memory::read< uintptr_t >( mm_get_virtual_for_physical + 0x10 ) ) );
				}

				const auto pfn = reinterpret_cast< pmmpfn_t >( ( uintptr_t )mm_pfn_database + ( phys_address >> 12 ) * sizeof( mmpfn_t ) );

				if ( pfn->u4.m_pte_frame != ( phys_address >> 12 ) )
					continue;

				size_t read{ };
				mmpte_t pml4e{ };

				if ( !physical::read_phys( phys_address + 8 * virt_addr.m_pml4_index, &pml4e, 8, &read ) )
					continue;

				if ( !pml4e.u.m_hard.m_valid )
					continue;

				mmpte_t pdpte{ };

				if ( !physical::read_phys( ( pml4e.u.m_hard.m_page_frame_number << 12 ) + 8 * virt_addr.m_pdpt_index, &pdpte, 8, &read ) )
					continue;

				if ( !pdpte.u.m_hard.m_valid )
					continue;

				mmpte_t pde{ };

				if ( !physical::read_phys( ( pdpte.u.m_hard.m_page_frame_number << 12 ) + 8 * virt_addr.m_pd_index, &pde, 8, &read ) )
					continue;

				if ( !pde.u.m_hard.m_valid )
					continue;

				mmpte_t pte{ };

				if ( !physical::read_phys( ( pde.u.m_hard.m_page_frame_number << 12 ) + 8 * virt_addr.m_pt_index, &pte, 8, &read ) )
					continue;

				if ( !pte.u.m_hard.m_valid )
					continue;

				call( ExFreePoolWithTag, ranges, 0 );
				cached_pid = pid;
				cached_dtb = phys_address;
				return phys_address;
			}
		}

		call( ExFreePoolWithTag, ranges, 0 );
		return 0;
	}

	bool read( uint32_t pid, uintptr_t address, void* buffer, size_t size ) {
		if ( !address
			 || !buffer
			 || !size )
			return false;

		const auto dtb = get_process_dtb( pid );

		if ( !dtb )
			return false;

		size_t current{ };
		size_t total_size{ size };

		while ( total_size ) {
			int64_t phys_address = physical::virt_to_phys( dtb, address + current );

			if ( !phys_address )
				return false;

			size_t read{ };
			const auto result = physical::read_phys( phys_address, reinterpret_cast< void* >( ( uintptr_t )buffer + current ), min( PAGE_SIZE - ( address & 0xFFF ), total_size ), &read );

			total_size -= read;
			current += read;

			if ( !result
				 || !read )
				break;
		}

		return true;
	}

	bool write( uint32_t pid, uintptr_t address, void* buffer, size_t size ) {
		if ( !address
			 || !buffer
			 || !size )
			return false;

		const auto dtb = get_process_dtb( pid );

		if ( !dtb )
			return false;

		size_t current{ };
		size_t total_size{ size };

		while ( total_size ) {
			int64_t phys_address = physical::virt_to_phys( dtb, address + current );

			if ( !phys_address )
				return false;

			size_t written{ };
			const auto result = physical::write_phys( phys_address, reinterpret_cast< void* >( ( uintptr_t )buffer + current ), min( PAGE_SIZE - ( address & 0xFFF ), total_size ), &written );

			total_size -= written;
			current += written;

			if ( !result
				 || !written )
				break;
		}

		return true;
	}
}