#pragma once

#include <thread>
#include <functional>
#include <future>
#include <memory>

#include "utils/simple_types.h"
#include "utils/noncopyable.h"

namespace rcp
{
    //interface is the same with std::thread 
    class Thread
    {
    public:
        struct Callable
        {
            virtual ~Callable() = default;

            virtual void execute() = 0;
        };

        enum class StartStrategy
        {
            Wait,
            NoWait
        };

        using id = std::thread::id;
        using native_handle_type = std::thread::native_handle_type;

        Thread() noexcept;
        template< typename Function, typename ...Args>
        Thread( StartStrategy startStrategy, Function&& f, Args&&... args );
        template< typename Function, typename ...Args>
        Thread( Function&& f, Args&&... args );
        Thread( Thread&& other ) noexcept;
        Thread( const Thread& other ) = delete;
        ~Thread();

        static unsigned int hardware_concurrency() noexcept;

        bool joinable() const noexcept;
        id get_id() const noexcept;

        // Windows: Note, common::threads::Thread return full handle because uses ::CreateThread
        native_handle_type native_handle();

        void join();
        void detach();
        void terminate();
        void swap( Thread& other ) noexcept;

        Thread& operator=( Thread&& other ) noexcept;
        Thread& operator=( const Thread& rhs ) = delete;

        static ThreadID toThreadID( id threadId );

    private:
        std::pair<native_handle_type, id> initialize( std::unique_ptr<Callable>&& );
        void resetHandleAndId();
        void joinImpl();
        void detachImpl();
        void terminateImpl();
        static void waitForStarted( std::shared_ptr<std::promise<void>>& started );

        native_handle_type m_handle = {};
        id m_id = {};
    };

    //inline methods

    template< typename Function, typename ...Args>
    inline Thread::Thread( StartStrategy startStrategy, Function&& fn, Args&&... args )
    {
        struct CallableImpl: Callable
        {
            std::shared_ptr<std::promise<void>> started;
            Function callable;
            std::tuple<Args...> argumentsTuple;

            CallableImpl( std::shared_ptr<std::promise<void>>& ptrStarted, Function&& fn, Args&&... args )
                :   started( ptrStarted ),
                    callable( std::forward<Function>( fn ) ), 
                    argumentsTuple( std::forward<Args>( args )... )
            {
            }

            void execute() override
            {
                if( started )
                {
                    started->set_value();
                }

                std::apply( std::move( callable ), std::move( argumentsTuple ) );
            }
        };

        std::shared_ptr<std::promise<void>> started;

        if( startStrategy == StartStrategy::Wait )
        {
            started.reset( new std::promise<void>() );
        }

        std::tie( m_handle, m_id ) = initialize( std::make_unique<CallableImpl>( started, std::forward<Function>( fn ), std::forward<Args>( args )... ) );

        if( startStrategy == StartStrategy::Wait )
        {
            waitForStarted( started );
        }
    }

    template< typename Function, typename ...Args>
    inline Thread::Thread( Function&& fn, Args&&... args )
        : Thread( StartStrategy::NoWait, std::move( fn ), std::move( args )... )
    {
        static_assert( !std::is_same_v<std::decay_t<Function>, StartStrategy>, "First parameter must be a function" );
    }

    namespace this_thread
    {
        // Windows: Note, uses ::GetCurrentThread(). So, it MUST NOT used for "Wait..." functions because it's returns pseudo handle.
        // Use DuplicateHandle - https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-duplicatehandle
        // Also, see "To close a duplicated handle..." in same article
        Thread::native_handle_type native_handle();
    }

}
