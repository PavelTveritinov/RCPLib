#pragma once

#include <Windows.h>

#include <string>
#include <system_error>


namespace rcp::Win
{
    std::wstring SysErrorText( DWORD dwError = ::GetLastError() );
    std::string SysErrorTextA( DWORD dwError = ::GetLastError() );
    std::error_code MakeSysErrorCode( int code );
    void ThrowLastError();
    void CloseHandleAndCheck( HANDLE& handle );
};

