#pragma once

#include "utils/platform/types.hpp"

#include <bit>
#include <concepts>

// Убираем спам от GCC/Clang про смену ABI при динамическом AVX диспетчинге
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpsabi"
#endif

// Include architecture-specific headers for vector intrinsics.
#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>
#endif

// Cross-platform macros for inlining and compiler target management
#if defined(__GNUC__) || defined(__clang__)
#define UTILS_ALWAYS_INLINE __attribute__((always_inline)) inline

#define UTILS_TARGET_AVX2 \
__attribute__ ((target("avx2,bmi,bmi2,lzcnt")))
#define UTILS_TARGET_AVX512 \
__attribute__ ((target("avx512f,avx512bw,avx512vl")))

#elif defined(_MSC_VER)
#define UTILS_ALWAYS_INLINE __forceinline
#define UTILS_TARGET_AVX2
#define UTILS_TARGET_AVX512
#else
#define UTILS_ALWAYS_INLINE inline
#define UTILS_TARGET_AVX2
#define UTILS_TARGET_AVX512
#endif

// Комбинированные макросы для удобства внутри специализаций simd
#define UTILS_AVX2_INLINE   UTILS_ALWAYS_INLINE UTILS_TARGET_AVX2
#define UTILS_AVX512_INLINE UTILS_ALWAYS_INLINE UTILS_TARGET_AVX512

namespace utils::platform::simd
{

// =========================================================================
// HARDWARE BIT MANIPULATION (BMI1 / TZCNT wrappers)
// =========================================================================
namespace bits
{
template< typename T >
concept MaskType = std::same_as< T, uint32_t > || std::same_as< T, uint64_t >;

UTILS_ALWAYS_INLINE uint32_t Tzcnt( MaskType auto mask ) noexcept
{
    return static_cast< uint32_t >( std::countr_zero( mask ) );
}

UTILS_ALWAYS_INLINE auto ClearLowestBit( MaskType auto mask ) noexcept
{
    return mask & ( mask - 1 );
}
} // namespace bits

template< typename T, size_t N >
struct simd;

#if defined(__x86_64__) || defined(_M_AMD64)

// =========================================================================
// SPECIALIZATION: AVX2 (256-bit) for 32 chars
// =========================================================================
template<>
struct simd< char, 32 >
{
    __m256i reg;

    UTILS_AVX2_INLINE simd() noexcept : reg( _mm256_setzero_si256() ) {}
    UTILS_AVX2_INLINE simd( __m256i r ) noexcept : reg( r ) {}

    UTILS_AVX2_INLINE static simd load_unaligned( const void* ptr ) noexcept
    {
        return _mm256_loadu_si256( reinterpret_cast< const __m256i* >( ptr ) );
    }

    UTILS_AVX2_INLINE void store_unaligned( void* ptr ) const noexcept
    {
        _mm256_storeu_si256( reinterpret_cast< __m256i* >( ptr ), reg );
    }

    UTILS_AVX2_INLINE static simd broadcast( char c ) noexcept
    {
        return _mm256_set1_epi8( c );
    }

    UTILS_AVX2_INLINE simd operator&( const simd& other ) const noexcept
    {
        return _mm256_and_si256( reg, other.reg );
    }

    UTILS_AVX2_INLINE simd operator|( const simd& other ) const noexcept
    {
        return _mm256_or_si256( reg, other.reg );
    }

    UTILS_AVX2_INLINE simd operator^( const simd& other ) const noexcept
    {
        return _mm256_xor_si256( reg, other.reg );
    }

    UTILS_AVX2_INLINE simd operator~() const noexcept
    {
        return _mm256_xor_si256( reg, _mm256_set1_epi32( -1 ) );
    }

    UTILS_AVX2_INLINE simd operator==( const simd& other ) const noexcept
    {
        return _mm256_cmpeq_epi8( reg, other.reg );
    }

    UTILS_AVX2_INLINE simd operator>( const simd& other ) const noexcept
    {
        return _mm256_cmpgt_epi8( reg, other.reg );
    }

    UTILS_AVX2_INLINE uint32_t cmpeq_bitmask( const simd& other ) const noexcept
    {
        return static_cast< uint32_t >( _mm256_movemask_epi8( _mm256_cmpeq_epi8( reg, other.reg ) ) );
    }
};

// =========================================================================
// SPECIALIZATION: AVX-512BW (512-bit) for 64 chars
// =========================================================================
template<>
struct simd< char, 64 >
{
    __m512i reg;

    UTILS_AVX512_INLINE simd() noexcept : reg( _mm512_setzero_si512() ) {}
    UTILS_AVX512_INLINE simd( __m512i r ) noexcept : reg( r ) {}

    UTILS_AVX512_INLINE static simd load_unaligned( const void* ptr ) noexcept
    {
        return _mm512_loadu_si512( ptr );
    }

    UTILS_AVX512_INLINE void store_unaligned( void* ptr ) const noexcept
    {
        _mm512_storeu_si512( ptr, reg );
    }

    UTILS_AVX512_INLINE static simd broadcast( char c ) noexcept
    {
        return _mm512_set1_epi8( c );
    }

    UTILS_AVX512_INLINE simd operator&( const simd& other ) const noexcept
    {
        return _mm512_and_si512( reg, other.reg );
    }

    UTILS_AVX512_INLINE simd operator|( const simd& other ) const noexcept
    {
        return _mm512_or_si512( reg, other.reg );
    }

    UTILS_AVX512_INLINE simd operator^( const simd& other ) const noexcept
    {
        return _mm512_xor_si512( reg, other.reg );
    }

    UTILS_AVX512_INLINE simd operator~() const noexcept
    {
        return _mm512_xor_si512( reg, _mm512_set1_epi32( -1 ) );
    }

    UTILS_AVX512_INLINE simd operator==( const simd& other ) const noexcept
    {
        __mmask64 mask = _mm512_cmpeq_epi8_mask( reg, other.reg );
        return _mm512_mask_blend_epi8( mask, _mm512_setzero_si512(), _mm512_set1_epi8( -1 ) );
    }

    UTILS_AVX512_INLINE simd operator>( const simd& other ) const noexcept
    {
        __mmask64 mask = _mm512_cmpgt_epi8_mask( reg, other.reg );
        return _mm512_mask_blend_epi8( mask, _mm512_setzero_si512(), _mm512_set1_epi8( -1 ) );
    }

    UTILS_AVX512_INLINE uint64_t cmpeq_bitmask( const simd& other ) const noexcept
    {
        return _mm512_cmpeq_epi8_mask( reg, other.reg );
    }
};

#endif // x86_64

} // namespace utils::platform::simd

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
