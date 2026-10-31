#pragma once

#include "../../includes/includes.hpp"

#include "../definitions/definitions.hpp"
#include "../vcruntime/vcruntime.hpp"
#include "../hash/hash.hpp"
#include "../memory/memory.hpp"
#include "../winapi/winapi.hpp"

#define to_lower( chr ) ( ( chr >= ( char* )'A' && chr <= ( char* )'Z' ) ? ( chr + 32 ) : chr )

namespace importer {
	typedef struct _module_t {
		uintptr_t m_base;
		size_t m_size;
	} module_t, * pmodule_t;

	using zw_query_system_information_t = decltype( &ZwQuerySystemInformation );
	inline zw_query_system_information_t zw_query_system_information{ };

	inline uintptr_t kernel_base{ };
	inline size_t kernel_size{ };

	__forceinline uintptr_t get_kernel_base( ) {
		auto idt_base = reinterpret_cast< uintptr_t >( KeGetPcr( )->IdtBase );
		auto align_page = *reinterpret_cast< uintptr_t* >( idt_base + 4 ) >> 0xc << 0xc;

		for ( ; align_page; align_page -= 0x1000 ) {
			for ( auto idx{ 0 }; idx < 0x1000 - 0x7; idx++ ) {
				auto current_address = static_cast< int64_t >( align_page ) + idx;

				if ( ( *reinterpret_cast< uint8_t* >( current_address ) == 0x48 && *reinterpret_cast< uint8_t* >( current_address + 1 ) == 0x8D && *reinterpret_cast< uint8_t* >( current_address + 2 ) == 0x3D && *reinterpret_cast< uint8_t* >( current_address + 6 ) == 0xFF && *reinterpret_cast< uint8_t* >( current_address + 7 ) == 0x48 && *reinterpret_cast< uint8_t* >( current_address + 8 ) == 0x63 )
					 || ( *reinterpret_cast< uint8_t* >( current_address ) == 0x48 && *reinterpret_cast< uint8_t* >( current_address + 1 ) == 0x8D && *reinterpret_cast< uint8_t* >( current_address + 2 ) == 0x3D && *reinterpret_cast< uint8_t* >( current_address + 6 ) == 0xFF && *reinterpret_cast< uint8_t* >( current_address + 7 ) == 0x48 && *reinterpret_cast< uint8_t* >( current_address + 8 ) == 0x8B && *reinterpret_cast< uint8_t* >( current_address + 9 ) == 0x8C && *reinterpret_cast< uint8_t* >( current_address + 15 ) == 0xE8 )
					 || ( *reinterpret_cast< uint8_t* >( current_address ) == 0x4C && *reinterpret_cast< uint8_t* >( current_address + 1 ) == 0x8D && *reinterpret_cast< uint8_t* >( current_address + 2 ) == 0x3D && *reinterpret_cast< uint8_t* >( current_address + 6 ) == 0xFF && *reinterpret_cast< uint8_t* >( current_address + 7 ) == 0x48 && *reinterpret_cast< uint8_t* >( current_address + 8 ) == 0x98 ) ) {
					auto offset = *reinterpret_cast< int* >( current_address + 3 );
					auto base = ( current_address + offset + 7 );

					if ( !( base & 0xfff ) )
						return base;
				}
			}
		}

		return 0;
	}

	__forceinline module_t get_module_base( const char* name ) {
		module_t module{ };

		uint8_t buffer[ 32 ]{ };
		uint32_t size{ 32 };

		zw_query_system_information( system_module_information, buffer, size, reinterpret_cast< PULONG >( &size ) );

		const auto info = reinterpret_cast< prtl_process_modules_t >( malloc( size ) );

		if ( !info )
			return { };

		if ( !NT_SUCCESS( zw_query_system_information( system_module_information, info, size, reinterpret_cast< PULONG >( &size ) ) ) ) {
			free( info );
			return { };
		}

		for ( size_t idx{ }; idx < info->m_number_of_modules; ++idx ) {
			const auto mod = info->m_modules[ idx ];

			if ( !stricmp( to_lower( ( char* )mod.m_full_path_name + mod.m_offset_to_file_name ), name ) ) {
				module.m_base = reinterpret_cast< uintptr_t >( mod.m_image_base );
				module.m_size = mod.m_image_size;

				break;
			}
		}

		free( info );
		return module;
	}

	__forceinline uintptr_t find_export( uintptr_t hash ) {
		auto nt_headers = reinterpret_cast< IMAGE_NT_HEADERS64* >( kernel_base + reinterpret_cast< IMAGE_DOS_HEADER* >( kernel_base )->e_lfanew );

		if ( nt_headers->Signature != 0x4550 )
			return 0;

		const auto data_directory = reinterpret_cast< IMAGE_DATA_DIRECTORY* >( &nt_headers->OptionalHeader.DataDirectory[ 0 ] );

		if ( !data_directory->VirtualAddress
			 || !data_directory->Size )
			return 0;

		const auto export_directory = reinterpret_cast< IMAGE_EXPORT_DIRECTORY* >( kernel_base + data_directory->VirtualAddress );

		if ( !export_directory->AddressOfFunctions
			 || !export_directory->AddressOfNames
			 || !export_directory->AddressOfNameOrdinals )
			return 0;

		const auto rva_table = reinterpret_cast< uint32_t* >( kernel_base + export_directory->AddressOfFunctions );
		const auto name_table = reinterpret_cast< uint32_t* >( kernel_base + export_directory->AddressOfNames );
		const auto ordinal_table = reinterpret_cast< uint16_t* >( kernel_base + export_directory->AddressOfNameOrdinals );

		for ( auto idx{ 0 }; idx < export_directory->NumberOfNames; ++idx ) {
			auto name = reinterpret_cast< const char* >( kernel_base + name_table[ idx ] );

			if ( HASH_RT( name ) == hash )
				return kernel_base + rva_table[ ordinal_table[ idx ] ];
		}

		return 0;
	}

	__forceinline bool initialize( ) {
		kernel_base = get_kernel_base( );

		if ( !kernel_base )
			return false;

		const auto nt_headers = reinterpret_cast< IMAGE_NT_HEADERS64* >( kernel_base + reinterpret_cast< IMAGE_DOS_HEADER* >( kernel_base )->e_lfanew );

		if ( nt_headers->Signature != 0x4550 )
			return false;

		const auto optional_header = &nt_headers->OptionalHeader;
		kernel_size = optional_header->SizeOfImage;

		if ( !kernel_size )
			return false;

		zw_query_system_information = reinterpret_cast< zw_query_system_information_t >( find_export( HASH( "ZwQuerySystemInformation" ) ) );

		if ( !zw_query_system_information )
			return false;

		return true;
	}
}

#define take( name ) *reinterpret_cast< name##_t >( ( uintptr_t )o_##name ^ HASH( #name ) );

#define def( name ) using name##_t = decltype( &name ); inline name##_t o_##name = nullptr;
#define set( name ) auto name##_hash = HASH( #name ); o_##name = reinterpret_cast< name##_t >( importer::find_export( name##_hash ) ^ name##_hash )

#define call( name, ... ) reinterpret_cast< name##_t >( ( uintptr_t )o_##name ^ HASH( #name ) )( __VA_ARGS__ )
#define inlined( name, ... ) reinterpret_cast< decltype( &name ) >( importer::find_export( HASH( #name ) ) )( __VA_ARGS__ )

def( DbgPrintEx )
def( ExAllocatePoolWithTag )
def( ExFreePoolWithTag )
def( RtlUnicodeStringToAnsiString )
def( RtlInitUnicodeString )
def( RtlInitAnsiString )
def( RtlFreeUnicodeString )
def( RtlFreeAnsiString )
def( KeDelayExecutionThread )
def( MmIsAddressValid )
def( ObReferenceObjectByName )
def( MmAllocateContiguousMemory )
def( IoDriverObjectType )
def( MmCopyMemory )
def( MmUnmapIoSpace )
def( MmMapIoSpaceEx )
def( MmGetPhysicalMemoryRanges )
def( PsGetProcessSectionBaseAddress )
def( PsGetProcessPeb )
def( KeStackAttachProcess )
def( KeUnstackDetachProcess )
def( IofCompleteRequest )
def( PsLookupProcessByProcessId )
def( ObfDereferenceObject )
def( MmSectionObjectType )