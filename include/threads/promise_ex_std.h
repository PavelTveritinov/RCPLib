#pragma once

#include <future>

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
        std::promise<TType> m_promise;
    };

    template <typename TType>
    void PromiseExImpl<TType>::set( TType val )
    {
        m_promise.set_value( val );
    }

    template <typename TType>
    void PromiseExImpl<TType>::set_exception( std::exception_ptr p )
    {
        m_promise.set_exception( p );
    }

    template <typename TType>
    TType PromiseExImpl<TType>::get( std::thread::native_handle_type )
    {
        return m_promise.get_future().get();
    }
}
