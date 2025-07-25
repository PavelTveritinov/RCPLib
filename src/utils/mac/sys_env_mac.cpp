#include "utils/sys_env.h"
#include "utils/func_raii.h"
#include "platform/platform.h"

#include <cstdlib>
#include <stdlib.h>
#include <string.h>

extern char** environ;

namespace rcp::details
{
    std::string SysEnvProc::getA( const std::string& varName )
    {
        auto value = std::getenv( varName.c_str() );
        return value? value : "";
    }

    std::wstring SysEnvProc::getW( const std::string& varName )
    {
        return Pl::Utf82Wide( getA( varName ) );
    }

    void SysEnvProc::put( const std::string& varName, const std::string& value )
    {
        if( value.empty() )
        {
            ::unsetenv( varName.c_str() );
        }
        else
        {
            ::setenv( varName.c_str(), value.c_str(), 1 );
        }
    }

    void SysEnvProc::put( const std::string& varName, const std::wstring& value )
    {
        put( varName, Pl::Wide2Utf8( value ) );
    }

    EnvsW SysEnvProc::allW()
    {
        EnvsW retVal;

        if( environ )
        {
            for( char** current = environ; *current; ++current )
            {
                const char* pEqual = strchr( *current, '=' );

                if( pEqual )
                {
                    retVal.push_back( { Pl::Utf82Wide( std::string( *current, pEqual - *current ) ),
                                        Pl::Utf82Wide( std::string( pEqual + 1 ) ) } );
                }
            }
        }

        return retVal;
    }

}
