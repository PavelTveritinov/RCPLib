#pragma once

#include "win/sync.h"

#include <vector>
#include <stdexcept>
#include <system_error>

namespace rcp::details
{
    template <typename TType>
    class PromiseExImpl : Utils::noncopyable
    {
    public:
        PromiseExImpl() = default;

        void set( TType val );
        void set_exception( std::exception_ptr p );
        TType get( std::thread::native_handle_type threadHandle );

    private:
        [[noreturn]] void throwFatalError( DWORD event, const std::string& reason );

        Win::Event m_event;
        std::exception_ptr m_exception;
        TType m_val = 0;
    };

    template <typename TType>
    void PromiseExImpl<TType>::set( TType val )
    {
        m_val = val;
        m_event.Set();
    }

    template <typename TType>
    void PromiseExImpl<TType>::throwFatalError( DWORD event, const std::string& reason )
    {
        auto errMsg = fmt::format( "WaitForMultipleObjects() for event = `{}` failed: `{}`", event, reason );
        throw std::runtime_error( errMsg );
    }

    template <typename TType>
    void PromiseExImpl<TType>::set_exception( std::exception_ptr p )
    {
        m_exception = p;
    }

    template <typename TType>
    TType PromiseExImpl<TType>::get( std::thread::native_handle_type threadHandle )
    {
        if( m_exception )
        {
            std::rethrow_exception( m_exception );
        }

        std::vector<HANDLE> handles{ m_event.ObjHandle(), threadHandle };

        const DWORD eventsCount = handles.size();
        const DWORD waitResult = ::WaitForMultipleObjects( eventsCount, handles.data(), FALSE, INFINITE );

        if( waitResult == WAIT_OBJECT_0 )
        {
            // do nothing...
        }
        else if( waitResult == ( WAIT_OBJECT_0 + 1 ) )
        {
            throwFatalError( waitResult, "thread terminated" );
        }
        else if( waitResult >= WAIT_ABANDONED_0 && waitResult < WAIT_ABANDONED_0 + eventsCount )
        {
            throwFatalError( waitResult - WAIT_ABANDONED_0, "abandoned" );
        }
        else if( waitResult == WAIT_FAILED )
        {
            // We have to store last error first, because WaitForSingleObject may change it...
            auto lastError = ::GetLastError();

            // Let's check thread exists.
            // Because it's return WAIT_FAILED if thread does not exist which is incorrect for us:
            auto checkExist = ::WaitForSingleObject( threadHandle, 0 );

            if( checkExist != WAIT_OBJECT_0 )
            {
                throwFatalError( checkExist, "thread terminated" );
            }

            throwFatalError( waitResult, "failed: " + std::system_category().message( lastError ) );
        }
        else if( waitResult == WAIT_TIMEOUT )
        {
            throwFatalError( waitResult, "timeout" );
        }
        else
        {
            throwFatalError( waitResult, "returned unexpected value" );
        }

        return m_val;
    }
}
