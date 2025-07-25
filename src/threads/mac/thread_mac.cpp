#include "threads/thread.h"

#include <pthread.h>
#include <system_error>

#include <cassert>

namespace rcp
{
    namespace
    {

        void* threadProc( void* param )
        {
            auto ptr = std::unique_ptr<Thread::Callable>{ reinterpret_cast< Thread::Callable* >( param ) };
            ptr->execute();
            return 0;
        }

    } //anonymous namespace

    std::pair<Thread::native_handle_type, Thread::id> Thread::initialize( std::unique_ptr<Callable>&& callablePtr )
    {
        pthread_t handle;
        if( const auto error = ::pthread_create( &handle, 0, threadProc, callablePtr.get() ); error != 0 )
        {
            throw std::system_error( error, std::system_category(), "create thread failure" );
        }
        else
        {
            callablePtr.release();
        }
        
        Thread::id id;
        reinterpret_cast<pthread_t&>(id) = handle;
        
        return std::make_pair( handle, id );
    }

    void Thread::joinImpl()
    {
        void* retval = nullptr;
        [[maybe_unused]] int result = ::pthread_join( m_handle, &retval );
    }

    void Thread::detachImpl()
    {
        [[maybe_unused]] int result = ::pthread_detach( m_handle );
    }
    
    void Thread::terminateImpl()
    {
        [[maybe_unused]] int result = ::pthread_kill( m_handle, EXIT_FAILURE );
        
        joinImpl();
    }


    namespace this_thread
    {
        Thread::native_handle_type native_handle()
        {
            return ::pthread_self();
        }
    }

}
