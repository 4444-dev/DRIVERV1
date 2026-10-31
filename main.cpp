#include "includes/includes.hpp"
#include "core/core.hpp"
#include "dependencies/hash/hash.hpp"

NTSTATUS driver_entry( ) {
	if ( !core::initialize( ) )
		return STATUS_UNSUCCESSFUL;

	return STATUS_SUCCESS;
}