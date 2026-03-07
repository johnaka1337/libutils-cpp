#pragma once

#include "utils/platform/types.hpp"

// Include architecture-specific headers for compiler intrinsics.
// These are required on x86/x64 architectures to access CPUID and XGETBV.
#if defined(__x86_64__) || defined(_M_AMD64)
#if defined(_MSC_VER)
#include <intrin.h> // MSVC intrinsics
#else
#include <cpuid.h>  // GCC/Clang intrinsics
#endif
#endif




namespace utils::platform
{
namespace cpu
{
namespace details
{
#if defined(__x86_64__) || defined(_M_AMD64)
/**
 * @brief Cross-platform wrapper for the x86 `cpuid` instruction.
 * CPUID returns processor info and feature bits.
 *
 * @param info Output array where registers are dumped:
 * [0]=EAX, [1]=EBX, [2]=ECX, [3]=EDX.
 * @param func_id The main query category (loaded into EAX before call).
 * @param subfunc_id The sub-query category (loaded into ECX before call).
 */
inline void cpuid( int32_t info[ 4 ],
                   int32_t func_id,
                   int32_t subfunc_id = 0 ) noexcept
{
#if defined(_MSC_VER)
    // MSVC intrinsic requires an int* array. The reinterpret_cast is
    // strictly safe here because 'int' is guaranteed to be 32-bit on
    // Windows platforms.
    __cpuidex( reinterpret_cast< int* >( info ), func_id, subfunc_id );
#else
    __cpuid_count( func_id,
                   subfunc_id,
                   info[ 0 ],
                   info[ 1 ],
                   info[ 2 ],
                   info[ 3 ] );
#endif
}


/**
 * @brief Cross-platform wrapper for the x86 `xgetbv` instruction.
 * Reads the Extended Control Register (XCR0). This is necessary to
 * verify if the Operating System actually preserves wide SIMD registers
 * during context switches. Silicon support is not enough; if the OS
 * doesn't save the state, using AVX will corrupt data.
 */
inline uint64_t xgetbv( ) noexcept
{
#if defined(_MSC_VER)
    return _xgetbv( 0 ); // 0 specifies XCR0
#else
    uint32_t eax, edx;
    // Execute xgetbv with ECX=0. The 64-bit result is split
    // between EDX (high) and EAX (low).
    __asm__ __volatile__( "xgetbv" : "=a"( eax ), "=d"( edx ) : "c"( 0 ) );
    return (static_cast< uint64_t >( edx ) << 32) | eax;
#endif
}
} // namespace details



/**
 * @brief Evaluates and caches CPU features at runtime.
 * This struct performs the actual hardware capability detection.
 */
struct features
{
    inline static bool os_uses_xsave{};
    inline static bool os_saves_ymm{ false };
    inline static bool os_saves_zmm{ false };

    inline static bool avx2{ false };
    inline static bool avx512bw{ false };




    features( const features& ) = delete;
    features( features&& ) noexcept = delete;
    features& operator = ( const features& ) = delete;
    features& operator = ( features&& ) noexcept = delete;
    ~features() = default;


private:
    features( ) noexcept
    {
        using details::cpuid;
        using details::xgetbv;

        // 1. Query basic CPU features and OS XSAVE support
        int32_t info[ 4 ];
        cpuid( info, 1 );

        // Bit 27 in ECX indicates if the OS supports the XSAVE/XRSTOR
        // instructions.
        // If false, the OS cannot manage extended CPU states,
        // so the initialization must abort.
        os_uses_xsave = (info[ 2 ] & (1 << 27)) != 0;
        if( ! os_uses_xsave )
        {
            return;
        }


        // 2. Query the OS to see WHICH registers it actively preserves
        const uint64_t xcr0 = xgetbv();

        // 0b110 checks bits 1 and 2:
        // OS preserves XMM (SSE) and YMM (AVX) state.
        os_saves_ymm = (xcr0 & 0b110) == 0b110;

        // 0b11100000 checks bits 5, 6, and 7: OS preserves Opmask (k0-k7),
        // ZMM_Hi256, and Hi16_ZMM state (required for AVX-512).
        os_saves_zmm = (xcr0 & 0b11100000) == 0b11100000;


        // 3. Query the physical silicon for specific instruction sets
        // Leaf 7, Sub-leaf 0 contains extended feature flags.
        cpuid(info, 7, 0);

        // AVX2 requires both OS YMM support and the AVX2 hardware flag
        // (Bit 5 in EBX).
        if( os_saves_ymm && (info[ 1 ] & (1 << 5)) )
        {
            avx2 = true;
        }

        // AVX-512 Byte/Word requires OS ZMM support, base AVX512 Foundation
        // (Bit 16 in EBX), and the specific AVX512BW flag (Bit 30 in EBX).
        if( os_saves_zmm && (info[ 1 ] & (1 << 16)) &&
            (info[ 1 ] & (1 << 30)) )
        {
            avx512bw = true;
        }
    }


    static const features instance_;
};
#else
// Fallback layer for non-x86 architectures (e.g., ARM64 / Apple Silicon).
struct features
{
    inline static bool os_uses_xsave{ false };
    inline static bool os_saves_ymm{ false };
    inline static bool os_saves_zmm{ false };

    inline static bool avx2{ false };
    inline static bool avx512bw{ false };

    features( const features& ) = delete;
    features( features&& ) noexcept = delete;
    features& operator = ( const features& ) = delete;
    features& operator = ( features&& ) noexcept = delete;
    ~features() = default;


private:
    features() noexcept = default;


    static const features instance_;
};
#endif


inline const features features::instance_{};
} // namespace cpu
} // namespace utils::platform
