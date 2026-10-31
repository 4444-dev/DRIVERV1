#include "core.hpp"

#include "../globals/globals.hpp"
#include "../handler/handler.hpp"

#include "../dependencies/pattern/pattern.cpp"
#include "../dependencies/importer/importer.hpp"
#include "../dependencies/winapi/winapi.hpp"
#include "../dependencies/encrypt/encrypt.hpp"
#include "../dependencies/pe/pe.hpp"

namespace core {
	bool initialize( ) {
		if ( !importer::initialize( ) )
			return false;

		set( DbgPrintEx );
		set( ExAllocatePoolWithTag );
		set( ExFreePoolWithTag );
		set( RtlUnicodeStringToAnsiString );
		set( RtlInitUnicodeString );
		set( RtlInitAnsiString );
		set( RtlFreeUnicodeString );
		set( KeDelayExecutionThread );
		set( RtlFreeAnsiString );
		set( MmIsAddressValid );
		set( ObReferenceObjectByName );
		set( MmAllocateContiguousMemory );
		set( MmCopyMemory );
		set( MmUnmapIoSpace );
		set( MmMapIoSpaceEx );
		set( MmGetPhysicalMemoryRanges );
		set( PsGetProcessSectionBaseAddress );
		set( PsGetProcessPeb );
		set( KeStackAttachProcess );
		set( KeUnstackDetachProcess );
		set( IofCompleteRequest );
		set( PsLookupProcessByProcessId );
		set( IoDriverObjectType );

		const auto ndis = importer::get_module_base( e( "ndis.sys" ) );

		if ( !ndis.m_base ) {
			call( DbgPrintEx, 0, 0, e( "[-] Failed to find text ndis.sys\n" ) );
			return false;
		}

		const auto text = pe::get_section( ndis.m_base, e( ".text" ), 5 );

		if ( !text.m_base ) {
			call( DbgPrintEx, 0, 0, e( "[-] Failed to find text section\n" ) );
			return false;
		}

		uintptr_t trampoline{ };

		trampoline = pattern::find( text.m_base, text.m_size, e( "\x48\x8B\x41\x08\x48\x8B\x40\x48\xFF\x15\x00\x00\x00\x00" ), e( "xxxxxxxxxx????" ) );

		if ( !trampoline ) {
			trampoline = pattern::find( text.m_base, text.m_size, e( "\x48\x8B\x41\x08\x48\x8B\x40\x48\xE8\x00\x00\x00\x00" ), e( "xxxxxxxxx????" ) );

			if ( !trampoline ) {
				call( DbgPrintEx, 0, 0, e( "[-] Failed to find trampoline\n" ) );
				return false;
			}
		}

		call( DbgPrintEx, 0, 0, e( "[+] Found trampoline in ndis.sys @ 0x%p\n" ), trampoline );

		globals::trampoline = trampoline;

		if ( !handler::initialize( ) )
			return false;

		return true;
	}
}