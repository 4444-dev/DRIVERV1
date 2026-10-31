#include "handler.hpp"

#include "../globals/globals.hpp"

#include "../dependencies/memory/memory.hpp"
#include "../dependencies/importer/importer.hpp"
#include "../dependencies/encrypt/encrypt.hpp"
#include "../dependencies/process/process.hpp"

#define _read CTL_CODE( FILE_DEVICE_UNKNOWN, 0x12bac, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )
#define _write CTL_CODE( FILE_DEVICE_UNKNOWN, 0xb2aa4, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )
#define _get_base_address CTL_CODE( FILE_DEVICE_UNKNOWN, 0xcc051, METHOD_BUFFERED, FILE_SPECIAL_ACCESS )

NTSTATUS finish( PDEVICE_OBJECT device_object, PIRP irp, NTSTATUS status ) {
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = 0;

	call( IofCompleteRequest, irp, IO_NO_INCREMENT );

	return status;
}

namespace handler {
	namespace impl {
		NTSTATUS hk_ndis_control( PDEVICE_OBJECT device, PIRP irp ) {
			const auto stack = IoGetCurrentIrpStackLocation( irp );

			if ( !stack )
				return o_ndis_control( device, irp );

			if ( stack->Parameters.DeviceIoControl.IoControlCode == _get_base_address ) {
				const auto buffer = reinterpret_cast< pget_base_address_t >( irp->AssociatedIrp.SystemBuffer );

				if ( !buffer )
					return finish( device, irp, STATUS_UNSUCCESSFUL );

				const auto address = process::get_base_address( buffer->m_pid );

				if ( !address )
					return finish( device, irp, STATUS_UNSUCCESSFUL );

				RtlCopyMemory( buffer->m_address, &address, sizeof( address ) );

				return finish( device, irp, STATUS_SUCCESS );
			} else if ( stack->Parameters.DeviceIoControl.IoControlCode == _read ) {
				const auto buffer = reinterpret_cast< pread_t >( irp->AssociatedIrp.SystemBuffer );

				if ( !buffer )
					return finish( device, irp, STATUS_UNSUCCESSFUL );

				if ( !process::read( buffer->m_pid, buffer->m_address, ( void* )buffer->m_buffer, buffer->m_size ) )
					return finish( device, irp, STATUS_UNSUCCESSFUL );

				return finish( device, irp, STATUS_SUCCESS );
			} else if ( stack->Parameters.DeviceIoControl.IoControlCode == _write ) {
				const auto buffer = reinterpret_cast< pwrite_t >( irp->AssociatedIrp.SystemBuffer );

				if ( !buffer )
					return finish( device, irp, STATUS_UNSUCCESSFUL );

				if ( !process::write( buffer->m_pid, buffer->m_address, ( void* )buffer->m_buffer, buffer->m_size ) )
					return finish( device, irp, STATUS_UNSUCCESSFUL );

				return finish( device, irp, STATUS_SUCCESS );
			}

			return o_ndis_control( device, irp );
		}
	}

	bool initialize( ) {
		if ( !memory::hook( e( L"\\Driver\\ndis" ), reinterpret_cast< uintptr_t >( impl::hk_ndis_control ), impl::o_ndis_control ) )
			return false;

		return true;
	}
}