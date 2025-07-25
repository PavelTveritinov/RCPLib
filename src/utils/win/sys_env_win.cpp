#include "utils/sys_env.h"
#include "utils/func_raii.h"
#include "platform/platform.h"

#include <Windows.h>
#include <processenv.h>

namespace rcp::details
{
    std::string SysEnvProc::getA( const std::string& varName )
    {
        auto newSize = ::GetEnvironmentVariableA( varName.c_str(), nullptr, 0 );

        if( newSize < 1 )
        {
            return "";
        }

        std::string retVal;
        retVal.resize( newSize - 1 );

        if( ::GetEnvironmentVariableA( varName.c_str(), retVal.data(), newSize ) )
        {
            return retVal;
        }

        return "";
    }

    std::wstring SysEnvProc::getW( const std::string& varName )
    {
        const auto varNameW = Pl::Ansii2Wide( varName );

        auto newSize = ::GetEnvironmentVariableW( varNameW.c_str(), nullptr, 0 );

        if( newSize < 1 )
        {
            return L"";
        }

        std::wstring retVal;
        retVal.resize( newSize - 1 );

        if( ::GetEnvironmentVariableW( varNameW.c_str(), retVal.data(), newSize ) )
        {
            return retVal;
        }

        return L"";
    }

    void SysEnvProc::put( const std::string& varName, const std::string& value )
    {
        ::SetEnvironmentVariableA( varName.c_str(), value.empty() ? nullptr : value.c_str() );
    }

    void SysEnvProc::put( const std::string& varName, const std::wstring& value )
    {
        const auto varNameW = Pl::Ansii2Wide( varName );
        ::SetEnvironmentVariableW( varNameW.c_str(), value.empty() ? nullptr : value.c_str() );
    }

    EnvsW SysEnvProc::allW()
    {
        auto* envsW = ::GetEnvironmentStringsW();

        if( !envsW )
        {
            return {};
        }

        FuncRAII atEnd( [&envsW]() { ::FreeEnvironmentStringsW( envsW ); } );

        EnvsW retVal;

        auto* current = envsW;
        while( *current )
        {
            std::wstring item( current );

            auto pos = item.find( L'=' );

            if( pos != std::wstring::npos )
            {
                auto varName = item.substr( 0, pos );

                if( !varName.empty() )
                {
                    retVal.push_back( { varName, item.substr( pos + 1 ) } );
                }
            }

            current += item.length() + 1;
        }

        return retVal;
    }

}
