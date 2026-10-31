#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>

namespace driver {
    namespace ioctl {
        constexpr ULONG get_base_address = CTL_CODE( FILE_DEVICE_UNKNOWN, 0xcc051, METHOD_BUFFERED, FILE_SPECIAL_ACCESS ); // 0x330144
        constexpr ULONG read_memory      = CTL_CODE( FILE_DEVICE_UNKNOWN, 0x12bac, METHOD_BUFFERED, FILE_SPECIAL_ACCESS ); // 0x26aeb0
        constexpr ULONG write_memory     = CTL_CODE( FILE_DEVICE_UNKNOWN, 0xb2aa4, METHOD_BUFFERED, FILE_SPECIAL_ACCESS ); // 0x2eaa90
    }

    struct get_base_address_t {
        uint32_t   m_pid;
        uintptr_t* m_address;
    };

    struct read_t {
        uint32_t  m_pid;
        uintptr_t m_address;
        uintptr_t m_buffer;
        uintptr_t m_size;
    };

    struct write_t {
        uint32_t  m_pid;
        uintptr_t m_address;
        uintptr_t m_buffer;
        uintptr_t m_size;
    };

    class client_t {
    private:
        HANDLE   m_handle{ INVALID_HANDLE_VALUE };
        uint32_t m_pid{ 0 };
        uintptr_t m_base_address{ 0 };

    public:
        client_t( ) = default;

        ~client_t( ) {
            disconnect( );
        }

        client_t( const client_t& ) = delete;
        client_t& operator=( const client_t& ) = delete;

        client_t( client_t&& other ) noexcept {
            m_handle = other.m_handle;
            m_pid = other.m_pid;
            m_base_address = other.m_base_address;

            other.m_handle = INVALID_HANDLE_VALUE;
            other.m_pid = 0;
            other.m_base_address = 0;
        }

        client_t& operator=( client_t&& other) noexcept {
            if ( this != &other ) {
                disconnect( );

                m_handle = other.m_handle;
                m_pid = other.m_pid;
                m_base_address = other.m_base_address;

                other.m_handle = INVALID_HANDLE_VALUE;
                other.m_pid = 0;
                other.m_base_address = 0;
            }
            return *this;
        }

        bool connect( const wchar_t* device_name = L"\\\\.\\ndis" ) {
            if ( is_connected( ) )
                return true;

            m_handle = CreateFileW(
                device_name,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                0,
                nullptr
            );

            return is_connected( );
        }

        void disconnect( ) {
            if ( is_connected( ) ) {
                CloseHandle( m_handle );
                m_handle = INVALID_HANDLE_VALUE;
            }
            m_pid = 0;
            m_base_address = 0;
        }

        [[nodiscard]] bool is_connected( ) const noexcept {
            return m_handle != INVALID_HANDLE_VALUE && m_handle != nullptr;
        }

        [[nodiscard]] uint32_t get_pid_by_name( const wchar_t* process_name ) const {
            uint32_t pid{ 0 };

            HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
            if ( snapshot == INVALID_HANDLE_VALUE )
                return 0;

            PROCESSENTRY32W entry{ };
            entry.dwSize = sizeof( entry );

            if ( Process32FirstW( snapshot, &entry ) ) {
                do {
                    if ( _wcsicmp( entry.szExeFile, process_name ) == 0 ) {
                        pid = entry.th32ProcessID;
                        break;
                    }
                } while ( Process32NextW( snapshot, &entry ) );
            }

            CloseHandle( snapshot );
            return pid;
        }

        bool attach( uint32_t pid ) {
            if ( !pid )
                return false;

            m_pid = pid;
            m_base_address = fetch_base_address( m_pid );
            return m_base_address != 0;
        }

        bool attach( const wchar_t* process_name ) {
            const auto pid = get_pid_by_name( process_name );
            if ( !pid )
                return false;

            return attach( pid );
        }

        uintptr_t fetch_base_address( uint32_t pid ) {
            if ( !is_connected( ) || !pid )
                return 0;

            uintptr_t base{ 0 };
            get_base_address_t request{ };
            request.m_pid = pid;
            request.m_address = &base;

            DWORD bytes_returned{ 0 };
            const BOOL success = DeviceIoControl(
                m_handle,
                ioctl::get_base_address,
                &request,
                sizeof( request ),
                nullptr,
                0,
                &bytes_returned,
                nullptr
            );

            return ( success && base ) ? base : 0;
        }

        [[nodiscard]] uint32_t get_pid( ) const noexcept {
            return m_pid;
        }

        [[nodiscard]] uintptr_t get_base_address( ) const noexcept {
            return m_base_address;
        }

        bool read_raw( uintptr_t address, void* buffer, size_t size ) const {
            if ( !is_connected( ) || !m_pid || !address || !buffer || !size )
                return false;

            read_t request{ };
            request.m_pid = m_pid;
            request.m_address = address;
            request.m_buffer = reinterpret_cast< uintptr_t >( buffer );
            request.m_size = size;

            DWORD bytes_returned{ 0 };
            return DeviceIoControl(
                m_handle,
                ioctl::read_memory,
                &request,
                sizeof( request ),
                nullptr,
                0,
                &bytes_returned,
                nullptr
            ) != FALSE;
        }

        bool write_raw( uintptr_t address, const void* buffer, size_t size ) const {
            if ( !is_connected( ) || !m_pid || !address || !buffer || !size )
                return false;

            write_t request{ };
            request.m_pid = m_pid;
            request.m_address = address;
            request.m_buffer = reinterpret_cast< uintptr_t >( buffer );
            request.m_size = size;

            DWORD bytes_returned{ 0 };
            return DeviceIoControl(
                m_handle,
                ioctl::write_memory,
                &request,
                sizeof( request ),
                nullptr,
                0,
                &bytes_returned,
                nullptr
            ) != FALSE;
        }

        template< typename T >
        [[nodiscard]] T read( uintptr_t address ) const {
            static_assert( std::is_trivially_copyable_v< T >, "Type must be trivially copyable" );

            T value{ };
            read_raw( address, &value, sizeof( T ) );
            return value;
        }

        template< typename T >
        bool read( uintptr_t address, T& out ) const {
            static_assert( std::is_trivially_copyable_v< T >, "Type must be trivially copyable" );
            return read_raw( address, &out, sizeof( T ) );
        }

        template< typename T >
        bool write( uintptr_t address, const T& value ) const {
            static_assert( std::is_trivially_copyable_v< T >, "Type must be trivially copyable" );
            return write_raw( address, &value, sizeof( T ) );
        }

        std::string read_string( uintptr_t address, size_t max_length = 256 ) const {
            if ( !address || !max_length )
                return "";

            std::vector< char > buffer( max_length + 1, '\0' );
            if ( !read_raw( address, buffer.data( ), max_length ) )
                return "";

            return std::string( buffer.data( ) );
        }

        std::wstring read_wstring( uintptr_t address, size_t max_length = 256 ) const {
            if ( !address || !max_length )
                return L"";

            std::vector< wchar_t > buffer( max_length + 1, L'\0' );
            if ( !read_raw( address, buffer.data( ), max_length * sizeof( wchar_t ) ) )
                return L"";

            return std::wstring( buffer.data( ) );
        }
    };

    inline client_t& get( ) {
        static client_t instance;
        return instance;
    }
}
