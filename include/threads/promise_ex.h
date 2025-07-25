#pragma once

#include <thread>
#include <exception>
#include <future>

#include "utils/noncopyable.h"

#ifdef WIN32
#include "promise_ex_win.h"
#else //WIN32
#include "promise_ex_std.h"
#endif //WIN32

namespace rcp
{
    // This class designed to handle case when ExitProcess used on Windows.
    // In this case all threads (except called) is terminated.
    // So, you never get a result on std::promise.
    // But current implementation will throw exception in this case.

    template <typename TType>
    class PromiseEx : noncopyable
    {
    public:
        PromiseEx() = default;

        void set_value( TType val )
        {
            m_impl.set( val );
        }

        void set_exception( std::exception_ptr p )
        {
            m_impl.set_exception( p );
        }

        [[nodiscard]] TType get( std::thread::native_handle_type threadHandle )
        {
            return m_impl.get( threadHandle );
        }

    private:
        details::PromiseExImpl<TType> m_impl;
    };

    template <>
    class PromiseEx<void> : noncopyable
    {
    public:
        PromiseEx() = default;

        void set_value()
        {
            m_impl.set( 0 );
        }

        void set_exception( std::exception_ptr p )
        {
            m_impl.set_exception( p );
        }

        // PT! Windows: You should not use threadHandle from GetCurrentProcess or GetCurrentThread.
        // See comment to common::threads::this_thread::native_handle()

        void get( std::thread::native_handle_type threadHandle )
        {
            m_impl.get( threadHandle );
        }

    private:
        details::PromiseExImpl<int> m_impl;
    };
}
