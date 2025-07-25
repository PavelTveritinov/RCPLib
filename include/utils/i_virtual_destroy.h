#pragma once 

#include <functional>

namespace rcp
{
    class IVirtualDestroy 
	{
	public:
	  virtual ~IVirtualDestroy() {}

	};

    class IVirtualDestroyBVoidFn : public IVirtualDestroy
    {
        std::function<void()> m_fn;

    public:
        IVirtualDestroyBVoidFn( const std::function<void()>& fn)
            : m_fn( fn )
        {
        }

        ~IVirtualDestroyBVoidFn()
        {
            if( m_fn )
                m_fn();
        }
    };
	
} 
