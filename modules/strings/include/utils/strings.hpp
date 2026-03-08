#pragma once


#include "utils/strings/concepts.hpp"
#include "utils/platform.hpp"

#include <string>
#include <string_view>
#include <array>



namespace utils::strings
{
template< CharType T = char, platform::size_t SZ = 64 >
class String
{
    using size_t = platform::size_t;
    union memblock
    {
        T* ptr_;
        std::array<T, SZ> arr_;

        memblock(): ptr_{nullptr} {arr_.fill(0);};
        ~memblock() {}
    } data_;

    size_t size_;


public:
    ~String()
    {
        if( size_ >= SZ )
        {
            delete[] data_.ptr_;
        }
    }


    String( const T* raw )
        :data_{}, size_{}
    {
        const T* tmp = raw;
        while( *tmp++ ) ++size_;
        if( size_ >= SZ )
        {
            data_.ptr_ = new T[ size_ + 1];
            for( size_t i = 0; i < size_; ++i )
                data_.ptr_[ i ] = *(raw + i);
            data_.ptr_[ size_ ] = 0;
        }
        else
        {
            for( size_t i = 0; i < size_; ++i )
                data_.arr_[ i ] = *(raw + i);
            data_.arr_[ size_ ] = 0;
        }
    }


    auto length() const { return size_; }


    operator const T*( ) const
    {
        return size_ > SZ ? data_.ptr_ : data_.arr_.data();
    }
};

} // namespace utils::strings
