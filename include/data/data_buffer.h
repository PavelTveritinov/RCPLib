#pragma once

#include <vector>
#include <memory>
#include <string>

namespace rcp
{
    class Buffer
    {
    public:
        using value_type = std::byte;

        virtual ~Buffer() = default;

        virtual value_type* data() = 0;
        virtual const value_type* data() const = 0;
        virtual std::size_t size() const = 0;
    };

    class VectorBuffer final : public Buffer
    {
    public:
        explicit VectorBuffer( std::vector<value_type> && data ) :  m_data( std::move( data ) ){}
        explicit VectorBuffer( std::size_t length ) : m_data( length ) {}

        value_type* data() { return m_data.data(); }
        const value_type* data() const { return m_data.data(); }
        std::size_t size() const { return m_data.size(); }

    private:
        std::vector< value_type > m_data;
    };

    class SharedBuffer final : public Buffer
    {
    public:
        explicit SharedBuffer( const std::shared_ptr<Buffer>& buffer ) : m_buffer( buffer ) {}
        explicit SharedBuffer( std::shared_ptr<Buffer> && buffer ) : m_buffer( std::move( buffer ) ) {}

        value_type* data() override { return m_buffer->data(); }
        const value_type* data() const override { return m_buffer->data(); }
        std::size_t size() const override { return m_buffer->size(); }

    private:
        std::shared_ptr< Buffer > m_buffer;
    };
    
    template< typename T >
    class TypedBuffer final: public Buffer
    {
    public:
        static_assert( std::is_trivial_v< T >, "type should be trivial" );
        
        TypedBuffer() = default;
        template< typename First, typename ...Left >
        explicit TypedBuffer( First && first, Left &&... left )
            : m_data( std::forward<First>( first ), std::forward<Left>( left )... )
        {}
        
        T & value() { return m_data; }
        const T& value() const { return m_data; }
        
        value_type* data() override { return reinterpret_cast<value_type*>( &m_data ); }
        const value_type* data() const override { return reinterpret_cast<const value_type*>( &m_data );  }
        std::size_t size() const override { return sizeof( m_data ); }
        
    private:
        T m_data;
    };

    template <typename TString>
    class StringBuffer final : public Buffer
    {
    public:
        explicit StringBuffer( TString&& data ) : m_data( std::move( data ) ) {}

        value_type* data() override { return reinterpret_cast<value_type*>( m_data.data() ); }
        const value_type* data() const override { return reinterpret_cast<const value_type*>( m_data.data() ); }
        std::size_t size() const override { return ( m_data.size() + 1 ) * sizeof( typename TString::value_type ); }

    private:
        TString m_data;
    };

    using StringBufferA = StringBuffer<std::string>;
    using StringBufferW = StringBuffer<std::wstring>;
}
