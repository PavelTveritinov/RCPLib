#pragma once

#include <string>

namespace rcp
{
    struct PlatformBase
    {
    private:

    public:
        static std::wstring Ansii2Wide(const std::string_view& /*in*/, bool /*throwOfFail*/ = true);
        static std::string Wide2Ansii(const std::wstring_view& /*in*/, bool /*throwOfFail*/ = true);

        static std::string Wide2Utf8( const std::wstring_view& in );
        static std::wstring Utf82Wide( const std::string_view& in );

        static std::string Ansii2Console( const std::string_view& in, bool throwOfFail = true );
        static std::string Wide2Console( const std::wstring_view& in, bool throwOfFail = true );

        static const std::wstring& EndlW();
        static const std::string& EndlA();
        static size_t EndlLength();
    };

    using Pl = PlatformBase;
}

