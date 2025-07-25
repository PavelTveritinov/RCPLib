#include "platform/platform.h"
#include "platform/utf8.h"

namespace rcp
{
    std::wstring PlatformBase::Ansii2Wide( const std::string_view& in, bool /*throwOfFail*/ )
    {
        return Utf82Wide( in );
    }

    std::string PlatformBase::Wide2Ansii( const std::wstring_view& in, bool /*throwOfFail*/ )
    {
        return Wide2Utf8( in );
    }

    std::string PlatformBase::Wide2Console( const std::wstring_view& in, bool )
    {
        return Wide2Utf8( in );
    }

    std::string PlatformBase::Ansii2Console( const std::string_view& in, bool )
    {
        return std::string( in );
    }

    const std::wstring& PlatformBase::EndlW()
    {
        static std::wstring endl( L"\n" );
        return endl;
    }

    const std::string& PlatformBase::EndlA()
    {
        static std::string endl( "\n" );
        return endl;
    }

    size_t PlatformBase::EndlLength()
    {
        return 1;
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
