#include "threads/thread.h"

#include "win/win_common.h"

#include <system_error>
#include <format>

namespace rcp
{
    namespace
    {
        DWORD WINAPI threadProc( LPVOID param )
        {
            auto ptr = std::unique_ptr<Thread::Callable>{ reinterpret_cast< Thread::Callable* >( param ) };
            ptr->execute();
            return 0;
        }

        void waitForThread( HANDLE& hThread )
        {
            auto waitResult = ::WaitForSingleObject( hThread, INFINITE );

            if( waitResult == WAIT_OBJECT_0 )
            {
                return;
            }

            std::string errText;

            if( waitResult == WAIT_FAILED )
            {
                auto lastError = Win::SysErrorTextA();

                // Let's check thread exists.
                // Because it's return WAIT_FAILED if thread does not exist:
                auto checkExist = ::WaitForSingleObject( hThread, 0 );

                if( checkExist != WAIT_OBJECT_0 )
                {
                    return;
                }

                errText = std::format( "{} failed with error WAIT_FAILED: {}", __func__, lastError );

            } else if( waitResult == WAIT_TIMEOUT )
            {
                errText = std::format( "{} failed with error WAIT_TIMEOUT. Impossible situation but happens...", __func__ );
            }
            else if( waitResult == WAIT_ABANDONED )
            {
                errText = std::format( "{} failed with error WAIT_ABANDONED (waiting for mutex). Impossible situation but happens...", __func__ );
            }

            if( errText.empty() )
            {
                errText = std::format( "{} failed with error {:#010x}. Impossible situation but happens...", __func__, static_cast< uint64_t >( waitResult ) );
            }

            throw std::runtime_error( errText.c_str() );
        }
    }

    void Thread::joinImpl()
    {
        waitForThread( m_handle );

        Win::CloseHandleAndCheck( m_handle );
    }

    void Thread::detachImpl()
    {
        Win::CloseHandleAndCheck( m_handle );
    }

    void Thread::terminateImpl()
    {
        if( !::TerminateThread( m_handle, EXIT_FAILURE ) )
        {
            auto errText = std::format( "{} failed with error: {}", __func__, Win::SysErrorTextA() );

            throw std::runtime_error( errText.c_str() );

            /* TODO!
            RcLog::RootNode()( RcLog::g_error, errText );
            RCV_DEBUG_ASSERT( false ) << errText;
            */
        }

        Win::CloseHandleAndCheck( m_handle );
    }

    std::pair<Thread::native_handle_type, Thread::id> Thread::initialize( std::unique_ptr<Callable>&& callablePtr )
    {
        DWORD dwordId;
        HANDLE handle = ::CreateThread(
            nullptr,
            0,
            &threadProc,
            callablePtr.get(),
            0,
            &dwordId
        );

        if ( handle == nullptr )
        {
            throw std::system_error( ::GetLastError(), std::system_category(), "create thread failure" );
        }
        else
        {
            callablePtr.release();
        }

        id threadId;
        reinterpret_cast<DWORD&>(threadId) = dwordId;
        
        return std::make_pair( handle, threadId );
    }

    namespace this_thread
    {
        Thread::native_handle_type native_handle()
        {
            return ::GetCurrentThread();
        }
    }

}
