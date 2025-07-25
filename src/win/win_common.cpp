#include "win/win_common.h"

#include <format>

namespace rcp::Win
{
    std::wstring SysErrorText( DWORD dwError )
    {
        LPWSTR lpMsgBuf = nullptr;

        ::FormatMessageW( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          0,
                          dwError,
                          MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
                          ( LPWSTR )&lpMsgBuf,
                          0,
                          0 );

        std::wstring retText = lpMsgBuf? lpMsgBuf : std::format( L"Unknown error {:#010x}", dwError );

        LocalFree( lpMsgBuf );

        return retText;
    }

    std::string SysErrorTextA( DWORD dwError )
    {
        LPSTR lpMsgBuf = nullptr;

        ::FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          0,
                          dwError,
                          MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
                          (LPSTR)&lpMsgBuf,
                          0,
                          0 );

        std::string retText = lpMsgBuf ? lpMsgBuf : std::format( "Unknown error {:#010x}", dwError );

        LocalFree( lpMsgBuf );

        return retText;
    }

    std::error_code MakeSysErrorCode( int code )
    {
        return std::error_code( code, std::system_category() );
    }

    void ThrowLastError()
    {
        auto lastError = ::GetLastError();
        auto errText = SysErrorTextA( lastError );
        auto errCode = MakeSysErrorCode( static_cast< int >( lastError ) );

        throw std::system_error( errCode, errText );
    }

    void CloseHandleAndCheck( HANDLE& handle )
    {
        if( !handle )
        {
            return;
        }

        if( ::CloseHandle( handle ) )
        {
            handle = nullptr;
            return;
        }

        auto errText = std::format( "{} failed with error: {}", __func__, SysErrorTextA() );

        throw std::runtime_error( errText.c_str() );

        /* TODO!
        RcLog::RootNode()( RcLog::g_error, errText );
        RCV_DEBUG_ASSERT( false ) << errText;
        */
    }

}

