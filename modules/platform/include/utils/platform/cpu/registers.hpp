#pragma once

#include "utils/platform/types.hpp"

#include <type_traits>
#include <cassert>




namespace utils::platform::cpu
{
/** Register type helper
 *  @tparam W width
 */
template< unsigned W >
using register_type_t =
    std::conditional_t< W == 64, UInt64,
                    std::conditional_t< W == 32, UInt32,
                    std::conditional_t< W == 16, UInt16,
                    std::conditional_t< W == 8,  UInt8, void > > > >;



/** Register type
 *  @tparam W width
 *  @tparam E endian (native by default)
 */
template< unsigned W, std::endian E = std::endian::native >
struct Register;



// 8-bit Specialization
template< std::endian E >
struct Register< 8, E >
{
    using Type = register_type_t< 8 >;
    Type value;


    explicit constexpr operator Type( ) const noexcept
    {
        return value;
    }


    constexpr bool operator [ ]( UInt8 pos ) const noexcept
    {
        assert( pos < 8 );
        return ( value >> pos ) & 1;
    }
};



// LITTLE-ENDIAN Specialization (x86_64, ARM)
template< unsigned W >
    requires ( W > (sizeof( UInt8 ) * 8) )
struct Register< W, std::endian::little >
{
    using Type = register_type_t< W >;
    using HalfType = Register< W / 2, std::endian::little >;

    union
    {
        Type value;
        struct
        {
            HalfType low;  // Little-Endian: low at first
            HalfType high;
        };
    };


    explicit constexpr operator Type( ) const noexcept
    {
        return value;
    }


    constexpr bool operator[ ]( UInt8 pos ) const noexcept
    {
        assert( pos < W );
        return ( value >> pos ) & 1;
    }
};



// BIG-ENDIAN Specialization
template< unsigned W >
    requires ( W > (sizeof( UInt8 ) * 8) )
struct Register< W, std::endian::big >
{
    using Type = register_type_t< W >;
    using HalfType = Register< W / 2, std::endian::big >;

    union
    {
        Type value;
        struct
        {
            HalfType high; // Big-Endian: high at fisrt
            HalfType low;
        };
    };


    explicit constexpr operator Type( ) const noexcept
    {
        return value;
    }


    constexpr bool operator[ ]( UInt8 pos ) const noexcept
    {
        assert( pos < W );
        return ( value >> pos ) & 1;
    }
};



// Bitwise operations
template< unsigned W, std::endian E >
constexpr Register< W, E > operator ~( Register< W, E > reg ) noexcept
{
    using Type = Register< W, E >::Type;
    return Register< W, E >{ static_cast< Type >( ~reg.value ) };
}

template< unsigned W, std::endian E >
constexpr Register< W, E > operator &( Register< W, E > lhs,
                                       Register< W, E > rhs ) noexcept
{
    using Type = Register< W, E >::Type;
    return Register< W, E >{ static_cast< Type >( lhs.value & rhs.value ) };
}

template< unsigned W, std::endian E >
constexpr Register< W, E >& operator &=( Register< W, E >& lhs,
                                         Register< W, E > rhs ) noexcept
{
    lhs.value &= rhs.value;
    return lhs;
}

template< unsigned W, std::endian E >
constexpr Register< W, E > operator |( Register< W, E > lhs,
                                       Register< W, E > rhs ) noexcept
{
    using Type = Register< W, E >::Type;
    return Register< W, E >{ static_cast< Type >( lhs.value | rhs.value ) };
}

template< unsigned W, std::endian E >
constexpr Register< W, E >& operator |=( Register< W, E >& lhs,
                                         Register< W, E > rhs ) noexcept
{
    lhs.value |= rhs.value;
    return lhs;
}

template< unsigned W, std::endian E >
constexpr Register< W, E > operator ^( Register< W, E > lhs,
                                       Register< W, E > rhs ) noexcept
{
    using Type = Register< W, E >::Type;
    return Register< W, E >{ static_cast< Type >( lhs.value ^ rhs.value ) };
}

template< unsigned W, std::endian E >
constexpr Register< W, E >& operator ^=( Register< W, E >& lhs,
                                         Register< W, E > rhs ) noexcept
{
    lhs.value ^= rhs.value;
    return lhs;
}

template< unsigned W, std::endian E >
constexpr Register< W, E > operator <<( Register< W, E > reg,
                                        UInt8 count ) noexcept
{
    assert( count < W );
    using Type = Register< W, E >::Type;
    return Register< W, E >{ static_cast< Type >( reg.value << count ) };
}

template< unsigned W, std::endian E >
constexpr Register< W, E >& operator <<=( Register< W, E >& reg,
                                          UInt8 count ) noexcept
{
    assert( count < W );
    reg.value <<= count;
    return reg;
}

template< unsigned W, std::endian E >
constexpr Register< W, E > operator >>( Register< W, E > reg,
                                        UInt8 count ) noexcept
{
    assert( count < W );
    using Type = Register< W, E >::Type;
    return Register< W, E >{ static_cast< Type >( reg.value >> count ) };
}

template< unsigned W, std::endian E >
constexpr Register< W, E >& operator >>=( Register< W, E >& reg,
                                          UInt8 count ) noexcept
{
    assert( count < W );
    reg.value >>= count;
    return reg;
}



// Aliases (native endian by default)
using Register8  = Register< 8 >;
using Register16 = Register< 16 >;
using Register32 = Register< 32 >;
using Register64 = Register< 64 >;

#if defined( __x86_64__ ) || defined( _M_X64 )

using XCR0 = Register64;
// using RAX = Register64;
// using RBX = Register64;
// using RCX = Register64;
// using RDX = Register64;


using EAX = Register32;
using EBX = Register32;
using ECX = Register32;
using EDX = Register32;

using AX = Register16;
using BX = Register16;
using CX = Register16;
using DX = Register16;

using AH = Register8;
using AL = Register8;

#elif defined( __aarch64__ ) || defined( _M_ARM64 )
using W0  = Register32;
using W1  = Register32;
using W2  = Register32;
using W3  = Register32;
using W4  = Register32;
using W5  = Register32;
using W6  = Register32;
using W7  = Register32;
using W8  = Register32;
using W9  = Register32;
using W10 = Register32;
using W11 = Register32;
using W12 = Register32;
using W13 = Register32;
using W14 = Register32;
using W15 = Register32;
using W16 = Register32;
using W17 = Register32;
using W18 = Register32;
using W19 = Register32;
using W20 = Register32;
using W21 = Register32;
using W22 = Register32;
using W23 = Register32;
using W24 = Register32;
using W25 = Register32;
using W26 = Register32;
using W27 = Register32;
using W28 = Register32;
using W29 = Register32;
using W30 = Register32;

using X0  = Register64;
using X1  = Register64;
using X2  = Register64;
using X3  = Register64;
using X4  = Register64;
using X5  = Register64;
using X6  = Register64;
using X7  = Register64;
using X8  = Register64;
using X9  = Register64;
using X10 = Register64;
using X11 = Register64;
using X12 = Register64;
using X13 = Register64;
using X14 = Register64;
using X15 = Register64;
using X16 = Register64;
using X17 = Register64;
using X18 = Register64;
using X19 = Register64;
using X20 = Register64;
using X21 = Register64;
using X22 = Register64;
using X23 = Register64;
using X24 = Register64;
using X25 = Register64;
using X26 = Register64;
using X27 = Register64;
using X28 = Register64;
using X29 = Register64;
using X30 = Register64;
#endif
} // namespace utils::platform::cpu
