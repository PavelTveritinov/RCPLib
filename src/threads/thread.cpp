#include "threads/thread.h"

#include <sstream>

#define THREAD_FORCE_CRASH *((volatile int*)nullptr) = 1

namespace rcp
{
    unsigned int Thread::hardware_concurrency() noexcept
    {
        return std::thread::hardware_concurrency();
    }

    Thread::Thread() noexcept = default;

    Thread::~Thread()
    {
        if ( joinable() )
        {
            THREAD_FORCE_CRASH; //like std::thread
        }
    }

    Thread::Thread( Thread&& other ) noexcept
    {
        swap( other );
    }

    Thread::id Thread::get_id() const noexcept
    {
        return m_id;
    }

    Thread::native_handle_type Thread::native_handle()
    {
        return m_handle;
    }

    bool Thread::joinable() const noexcept
    {
        return m_handle != nullptr;
    }

    void Thread::join()
    {
        if ( joinable() )
        {
            if ( std::this_thread::get_id() == m_id )
            {
                THREAD_FORCE_CRASH; //instead of infinite waiting
            }

            joinImpl();
            resetHandleAndId();
        }
    }

    void Thread::detach()
    {
        if ( joinable() )
        {
            detachImpl();
            resetHandleAndId();
        }
    }

    void Thread::terminate()
    {
        if ( joinable() )
        {
            terminateImpl();
            resetHandleAndId();
        }
    }

    void Thread::swap( Thread& other ) noexcept
    {
        std::swap( m_handle, other.m_handle );
        std::swap( m_id, other.m_id );
    }

    Thread& Thread::operator=( Thread&& other ) noexcept
    {
        if ( joinable() )
        {
            THREAD_FORCE_CRASH;
        }

        swap( other );
        return *this;
    }

    void Thread::resetHandleAndId()
    {
        m_handle = nullptr;
        m_id = {};
    }

    ThreadID Thread::toThreadID( id threadId )
    {
        std::stringstream str_hex;
        str_hex << std::hex << threadId;
        str_hex.seekg( 0 );

        ThreadID val;
        str_hex >> std::hex >> val;

        return val;
    }

    void Thread::waitForStarted( std::shared_ptr<std::promise<void>>& started )
    {
        started->get_future().get();
    }

}

#undef THREAD_FORCE_CRASH
