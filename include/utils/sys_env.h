#pragma once

#include "utils/num_traits.h"

#include <vector>
#include <utility>
#include <exception>
#include <cstddef>
#include <typeinfo>


namespace rcp
{
    namespace details
    {
        using EnvPairW = std::pair<std::wstring, std::wstring>;
        using EnvsW = std::vector<EnvPairW>;

        inline void logConversionTypeError( const std::string&, const std::string& )
        {
            // TODO
        }

        struct SysEnvProc
        {
            static std::string getA( const std::string& varName );
            static std::wstring getW( const std::string& varName );

            // putX MUST delete variable if value empty
            static void put( const std::string& varName, const std::string& value );
            static void put( const std::string& varName, const std::wstring& value );

            template <typename T>
            static void putStr( const std::string& envVarName, const typename T::value_type* val )
            {
                if( !val )
                {
                    logConversionTypeError( "Null pointer", typeid( val ).name() );
                }
                else
                {
                    T valDup( val );
                    put( envVarName, valDup );
                }
            }

            static EnvsW allW();
        };
    }

    namespace SysEnv
    {
        template <typename T>
        T defValue()
        {
            return NumTraits::defValue<T>();
        }

        template <>
        inline std::string defValue<std::string>()
        {
            return std::string();
        }

        template <>
        inline std::wstring defValue<std::wstring>()
        {
            return std::wstring();
        }

        template <typename T>
        T get( const std::string& envVarName, const T& defVal = defValue<T>() )
        {
            auto val = details::SysEnvProc::getA( envVarName );

            if( !val.empty() )
            {
                try
                {
                    return NumTraits::from<T>( val );
                }
                catch( const std::exception& e )
                {
                    details::logConversionTypeError( e.what(), typeid( T ).name() );
                }
            }

            return defVal;
        }

        template <>
        inline std::string get( const std::string& envVarName, const std::string& defVal )
        {
            auto val = details::SysEnvProc::getA( envVarName );

            if( val.empty() )
            {
                return defVal;
            }

            return val;
        }

        template <>
        inline std::wstring get( const std::string& envVarName, const std::wstring& defVal )
        {
            auto val = details::SysEnvProc::getW( envVarName );

            if( val.empty() )
            {
                return defVal;
            }

            return val;
        }

        template <typename T>
        void put( const std::string& envVarName, const T& val )
        {
            const auto valStr = std::to_string( val );
            details::SysEnvProc::put( envVarName, valStr );
        }

        template <>
        inline void put( const std::string& envVarName, const std::nullptr_t& )
        {
            details::SysEnvProc::put( envVarName, "" );
        }

        template <>
        inline void put( const std::string& envVarName, const std::string& val )
        {
            details::SysEnvProc::put( envVarName, val );
        }

        template <>
        inline void put( const std::string& envVarName, const std::wstring& val )
        {
            details::SysEnvProc::put( envVarName, val );
        }

        inline void put( const std::string& envVarName, const char* val )
        {
            details::SysEnvProc::putStr<std::string>( envVarName, val );
        }

        inline void put( const std::string& envVarName, const wchar_t* val )
        {
            details::SysEnvProc::putStr<std::wstring>( envVarName, val );
        }

        inline void clear( const std::string& envVarName )
        {
            put( envVarName, nullptr );
        }

        bool exists( const std::string& envVarName );

        std::wstring dump();
    }

    class EnvRAII
    {
    public:

        EnvRAII( const std::string& name, const std::string& val );
        ~EnvRAII();

    private:
        std::string m_name;
        std::string m_prevValue;
    };
}
