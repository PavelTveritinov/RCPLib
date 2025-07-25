#pragma once

#include <string>

namespace rcp
{
    std::string toUtf8( const std::wstring_view& src );
    std::wstring fromUtf8( const std::string_view& src );
}
