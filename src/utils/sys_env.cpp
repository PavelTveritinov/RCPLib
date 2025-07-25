#include "utils/sys_env.h"

#include <algorithm>

namespace rcp::SysEnv
{
    std::wstring dump()
    {
        auto allItems = details::SysEnvProc::allW();

        std::sort( allItems.begin(), 
                   allItems.end(), 
                   []( const details::EnvPairW& lhs, const details::EnvPairW& rhs )
                   {
                       return ( lhs.first.compare( rhs.first ) < 0 ) ? true : false;
                   } );

        std::wstring retStr;

        for( const auto& it : allItems )
            retStr += it.first + L"=" + it.second + L"\n";

        return retStr;
    }

    bool exists( const std::string& envVarName )
    {
        return !get<std::string>( envVarName ).empty();
    }
}

namespace rcp
{
    EnvRAII::EnvRAII( const std::string& name, const std::string& val )
        :   m_name( name ),
            m_prevValue( SysEnv::get<std::string>( name ) )
    {
        SysEnv::put( m_name, val );
    }

    EnvRAII::~EnvRAII()
    {
        try
        {
            // not throw exception in destructor
            SysEnv::put( m_name, m_prevValue );
        }
        catch( ... )
        {
        }
    }
}
