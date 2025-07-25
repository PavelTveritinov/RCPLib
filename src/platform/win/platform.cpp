#include "platform/platform.h"
#include "platform/utf8.h"
#include "win/win_common.h"

#include <Windows.h>

#include <vector>

namespace rcp
{
    void CheckThrowOfFail( bool throwOfFail, const std::string& func )
    {
        if( throwOfFail )
        {
            ::rcp::Win::ThrowLastError();
/* TODO:
            try
            {
                ::rcp::Win::ThrowLastError();
            }
            catch( const std::exception& )
            {
                auto callStack = PlatformBase::GetStackTrace();

                if( callStack.empty() )
                    callStack = "<unknown>";

                PlatformBase::ConsoleA()( ) << ( func + " failed with stack:\n" + callStack );

                throw;
            }
*/
        }

    }

    std::string Wide2AnsiiHelper( UINT codePage, const std::wstring_view& in, bool throwOfFail )
    {
        if( in.empty() )
            return "";

        std::vector<char> buff;

        auto fnCall = [ & ]( size_t size ) -> int
            {
                buff.resize( size );

                BOOL usedDefaultChar = false;

                return WideCharToMultiByte( codePage,
                                            0,
                                            in.data(),
                                            -1,
                                            buff.data(),
                                            static_cast< int >( buff.size() ),
                                            nullptr,
                                            ( codePage == CP_ACP ) ? &usedDefaultChar : nullptr );

            };

        bool callFailed = false;

        if( !fnCall( in.length() * 2 ) ) // Let's have doubled buffer at start
        {
            callFailed = true;

            if( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
            {
                auto necessaryBytes = fnCall( 0 );

                if( necessaryBytes > 0 )
                    callFailed = !fnCall( static_cast< size_t >( necessaryBytes ) );
            }
        }

        if( callFailed )
        {
            CheckThrowOfFail( throwOfFail, "Wide2AnsiiHelper" );

            return "";
        }

        return buff.data();
    }

    std::wstring Ansii2WideHelper( UINT codePage, const std::string_view& in, bool throwOfFail )
    {
        if( in.empty() )
            return L"";

        std::vector<wchar_t> buff( in.length() + 1 );

        auto res = MultiByteToWideChar( codePage, 0, in.data(), -1, buff.data(), static_cast< int >( buff.size() ) );

        if( !res )
        {
            CheckThrowOfFail( throwOfFail, "Ansii2WideHelper" );

            return L"";
        }

        return buff.data();
    }
}

namespace rcp
{
    std::wstring PlatformBase::Ansii2Wide(const std::string_view& in, bool throwOfFail)
    {
        return Ansii2WideHelper(CP_ACP, in, throwOfFail);
    }

    std::string PlatformBase::Wide2Console( const std::wstring_view& in, bool throwOfFail )
    {
        return Wide2AnsiiHelper( CP_OEMCP, in, throwOfFail );
    }

    std::string PlatformBase::Ansii2Console( const std::string_view& in, bool throwOfFail )
    {
        return Wide2Console( Ansii2WideHelper( CP_ACP, in, throwOfFail ), throwOfFail );
    }

    const std::wstring& PlatformBase::EndlW()
    {
        static std::wstring endl(L"\r\n");
        return endl;
    }

    const std::string& PlatformBase::EndlA()
    {
        static std::string endl("\r\n");
        return endl;
    }

    size_t PlatformBase::EndlLength()
    {
        return 2;
    }

    std::string PlatformBase::Wide2Ansii(const std::wstring_view& in, bool throwOfFail)
    {
        return Wide2AnsiiHelper(CP_ACP, in, throwOfFail);
    }

    std::string PlatformBase::Wide2Utf8( const std::wstring_view& in )
    {
        return toUtf8( in );
    }
    
    std::wstring PlatformBase::Utf82Wide( const std::string_view& in )
    {
        return fromUtf8( in );
    }

}
