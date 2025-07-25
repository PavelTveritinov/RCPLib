#pragma once

#include "utils/noncopyable.h"
#include "utils/i_virtual_destroy.h"
#include "win_common.h"

namespace rcp::Win
{
    class SyncObject : public IVirtualDestroy, noncopyable
    {
        HANDLE m_handle;

        friend class SyncLock;

    protected:

        virtual bool Lock( DWORD dwTimeout )
        {
            auto ret = ::WaitForSingleObject( m_handle, dwTimeout );

            if( ret == WAIT_OBJECT_0 || ret == WAIT_ABANDONED )
                return true;

            return FALSE;
        }

        virtual bool Unlock() = 0;

    public:

        SyncObject( HANDLE handle )
            : m_handle( handle )
        {
        }

        ~SyncObject() override
        {
            if( m_handle )
            {
                ::CloseHandle( m_handle );
                m_handle = nullptr;
            }
        }

        HANDLE ObjHandle() { return m_handle; }
    };

    class SyncLock : noncopyable
    {
        SyncObject& m_obj;

    public:
        SyncLock( SyncObject& obj )
            : m_obj( obj )
        {
            if( !m_obj.Lock( INFINITE ) )
                throw std::runtime_error( "Unable to lock sync object." );
        }

        ~SyncLock()
        {
            m_obj.Unlock();
        }
    };

    class CriticalSection : public SyncObject
    {
        CRITICAL_SECTION m_sect;

        bool Lock( DWORD ) override
        {
            ::EnterCriticalSection( &m_sect );

            return true;
        }

        bool Unlock() override
        {
            ::LeaveCriticalSection( &m_sect );

            return true;
        }

    public:
        CriticalSection()
            : SyncObject( nullptr )
        {
            if( !::InitializeCriticalSectionAndSpinCount( &m_sect, 0 ) )
                ThrowLastError();
        }

        ~CriticalSection() override
        {
            ::DeleteCriticalSection( &m_sect );
        }
    };

    class Event : public SyncObject
    {
        bool Unlock() override { return true; }

    public:
        Event( bool initiallyOwn = false,
               bool manualReset = false,
               const std::wstring& name = L"",
               LPSECURITY_ATTRIBUTES attributes = nullptr )
            : SyncObject( ::CreateEventW( attributes, 
                                          manualReset? TRUE : FALSE,
                                          initiallyOwn ? TRUE : FALSE,
                                          name.empty()? nullptr : name.c_str() ) )
        {
            if( !ObjHandle() )
                throw std::runtime_error( "Unable to create event object." );
        }

        bool Set() { return ::SetEvent( ObjHandle() ) ? true : false; }
        bool Pulse() { return ::PulseEvent( ObjHandle() ) ? true : false; }
        bool Reset() { return ::ResetEvent( ObjHandle() ) ? true : false; }
    };

};

