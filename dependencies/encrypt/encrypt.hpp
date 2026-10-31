#pragma once

#include "../../includes/includes.hpp"

namespace std {
	template< class _Ty >
	struct remove_reference {
		using type = _Ty;
	};

	template< class _Ty >
	struct remove_reference< _Ty& > {
		using type = _Ty;
	};

	template< class _Ty >
	struct remove_reference< _Ty&& > {
		using type = _Ty;
	};

	template< class _Ty >
	using remove_reference_t = typename remove_reference< _Ty >::type;

	template< class _Ty >
	struct remove_const {
		using type = _Ty;
	};

	template< class _Ty >
	struct remove_const< const _Ty > {
		using type = _Ty;
	};

	template< class _Ty >
	using remove_const_t = typename remove_const< _Ty >::type;
}

namespace encrypt {
	template< class _Ty >
	using clean_t = typename std::remove_const_t< std::remove_reference_t< _Ty > >;

	template< int m_size, char m_key1, char m_key2, typename _Ty >
	class encrypt_c {
	public:
		__forceinline constexpr encrypt_c( _Ty* data ) {
			crypt( data );
		}

		__forceinline _Ty* get( ) {
			return m_storage;
		}

		__forceinline int size( ) {
			return m_size;
		}

		__forceinline char key( ) {
			return m_key1;
		}

		__forceinline _Ty* encrypt( ) {
			if ( !is_encrypted( ) )
				crypt( m_storage );

			return m_storage;
		}

		__forceinline _Ty* decrypt( ) {
			if ( is_encrypted( ) )
				crypt( m_storage );

			return m_storage;
		}

		__forceinline bool is_encrypted( ) {
			return m_storage[ m_size - 1 ] != 0;
		}

		__forceinline void clear( ) {
			for ( auto idx{ 0 }; idx < m_size; idx++ ) {
				m_storage[ idx ] = 0;
			}
		}

		__forceinline operator _Ty* ( ) {
			decrypt( );
			return m_storage;
		}

	private:
		__forceinline constexpr void crypt( _Ty* data ) {
			for ( auto idx{ 0 }; idx < m_size; idx++ ) {
				m_storage[ idx ] = data[ idx ] ^ ( m_key1 + idx % ( 1 + m_key2 ) );
			}
		}

		_Ty m_storage[ m_size ]{ };
	};
}

#define e( buf ) key( buf, __TIME__[ 4 ], __TIME__[ 7 ] )
#define key( buf, key1, key2 ) [ ]( ) { constexpr static auto crypted = encrypt::encrypt_c< sizeof( buf ) / sizeof( buf[ 0 ] ), key1, key2, encrypt::clean_t< decltype( buf[ 0 ] ) > >( ( encrypt::clean_t< decltype( buf[ 0 ] ) >*)buf ); return crypted; }( )