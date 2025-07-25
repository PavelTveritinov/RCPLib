#pragma once

namespace rcp
{
    class noncopyable
    {
    protected:
        noncopyable() = default;
        noncopyable( const noncopyable& ) = delete;
        noncopyable( noncopyable&& ) = default;
        
        ~noncopyable() = default;

        noncopyable& operator=( const noncopyable& ) = delete;
        noncopyable& operator=( noncopyable&& ) = default;
    };
};
