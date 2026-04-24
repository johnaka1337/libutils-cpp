#pragma once

#include "utils/platform/cpu/registers.hpp"

// Include platform-specific headers for compiler intrinsics
#if defined( __x86_64__ ) || defined( _M_X64 )
#   if defined(__GNUC__) || defined(__clang__)
#       include <cpuid.h>
#   elif defined(_MSC_VER)
#       include <intrin.h>
#   endif
#   include <immintrin.h> // _xgetbv



namespace utils::platform::cpu
{

/**
 * @brief xgetbv    Uses for checking OS's XSAVE/XRSTOR
 * @param leaf      Currently supports 0 only
 * @return XCR0
 */
inline XCR0 xgetbv( UInt32 leaf = 0 ) noexcept
{
    XCR0 result{};

#if defined( __GNUC__ ) || defined( __clang__ )
    UInt32 a, d;

    __asm__ __volatile__(
        "xgetbv"
        : "=a"(a), "=d"(d)      // output data
        : "c"(leaf)             // input data
        );

    result.value = (static_cast< UInt64 >( d ) << 32) | a;

#elif defined( _MSC_VER )
    result.value = static_cast< uint64 >( _xgetbv( leaf ) );
#else
#error Unknown architecture or not implemented!
#endif

    return result;
}



/**
 * @brief Holds the state of the four general-purpose 32-bit registers
 * after executing the CPUID instruction.
 */
struct cpuid_result
{
    EAX eax;
    EBX ebx;
    ECX ecx;
    EDX edx;
};


/**
 * @brief Executes the x86 CPUID instruction to query processor information
 * and features.
 *
 * @param leaf    The main CPUID leaf number (loaded into the EAX register
 * before execution).
 * @param subleaf The subleaf or sub-function number (loaded into the ECX
 * register, defaults to 0).
 * @return        A `cpuid_result` struct containing the output values of
 * EAX, EBX, ECX, and EDX.
 */
inline cpuid_result cpuid( UInt32 leaf, UInt32 subleaf = 0 ) noexcept
{
    UInt32 regs[ 4 ] = { 0 };

#if defined(__GNUC__) || defined(__clang__)
    __cpuid_count( leaf, subleaf,
                  regs[ 0 ], regs[ 1 ], regs[ 2 ], regs[ 3 ] );
#elif defined(_MSC_VER)
    // MSVC provides an intrinsic function that expects an int[4] array ptr.
    __cpuidex( reinterpret_cast< int* >( regs ),
              static_cast< int >( leaf ), static_cast< int >( subleaf ) );
#endif

    return cpuid_result{ EAX{ regs[ 0 ] },
                         EBX{ regs[ 1 ] },
                         ECX{ regs[ 2 ] },
                         EDX{ regs[ 3 ] } };
}
} // namespace utils::platform::cpu
#endif // x86_64
