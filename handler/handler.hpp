#pragma once

#include "../includes/includes.hpp"

namespace handler {
	namespace impl {
		inline PDRIVER_DISPATCH o_ndis_control{ };

		NTSTATUS hk_ndis_control( PDEVICE_OBJECT, PIRP );
	}

	bool initialize( );
}