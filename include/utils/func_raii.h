#pragma once

#include <utility>
#include <functional>
#include "noncopyable.h"
#include "i_virtual_destroy.h"

namespace rcp
{
    class FuncRAII : public IVirtualDestroy, noncopyable
    {
        std::function<void()> m_fn;

    protected:

        void ResetFn()
        {
            m_fn = nullptr;
        }

    public:

        FuncRAII() = default;

        FuncRAII( const std::function<void()>& fn ) : m_fn( fn )
        {
        }

        FuncRAII( std::function<void()>&& fn ) : m_fn( std::move(fn) )
        {
        }

        ~FuncRAII()
        {
            if( m_fn )
                m_fn();
        }

        operator bool() const
        {
            return m_fn.operator bool();
        }
    };
}
