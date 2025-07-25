#pragma once

#include "always_false.h"

#include <string>

namespace rcp
{
    namespace NumTraits
    {
        template <typename T>
        T defValue()
        {
            return 0;
        }

        template <typename T>
        T from( const std::string& val )
        {
            static_assert( always_false_v<T>, "not implemented for type" );
        }

        template <>
        inline long long from<long long>( const std::string& val )
        {
            return std::stoull( val );
        }

        template <>
        inline unsigned long from<unsigned long>( const std::string& val )
        {
            return std::stoul( val );
        }

        template <>
        inline int from<int>( const std::string& val )
        {
            return std::stoi( val );
        }

        template <>
        inline long from<long>( const std::string& val )
        {
            return std::stol( val );
        }
    }
}
