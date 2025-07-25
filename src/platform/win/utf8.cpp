#include "platform/utf8.h"

#include <Windows.h>
#include <vector>

namespace rcp
{
    std::string toUtf8( const std::wstring_view& src )
    {
        int len = static_cast<int>( src.length() );
        if( len == 0 )
            return std::string();

        int charcount = ::WideCharToMultiByte( CP_UTF8, 0, src.data(), len, NULL, 0, NULL, NULL );

        if( charcount == 0 )
            return std::string();

        std::string mb;
        mb.resize( charcount );
        ::WideCharToMultiByte( CP_UTF8, 0, src.data(), len, &mb[ 0 ], charcount, NULL, NULL );

        return mb;
    };
    
    std::wstring fromUtf8( const std::string_view& src )
    {
        int len = static_cast<int>( src.length() );
        if( len == 0 )
            return std::wstring();

        int charcount = ::MultiByteToWideChar( CP_UTF8, 0, src.data(), len, NULL, 0 );

        if( charcount == 0 )
            return std::wstring();

        std::wstring wide;
        wide.resize( charcount );

        ::MultiByteToWideChar( CP_UTF8, 0, src.data(), len, &wide[ 0 ], charcount );

        return wide;
    };
}