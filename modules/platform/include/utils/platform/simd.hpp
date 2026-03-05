#pragma once


//#include "utils/platform/types.hpp"

// Include architecture-specific headers for vector intrinsics.
// These are required on x86/x64 architectures to access SIMD registers and
// instructions.
#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>
#endif



// Cross-platform macros for inlining and compiler target management
#if defined(__GNUC__) || defined(__clang__)
/**
 * @brief Forces the compiler to inline the function, ignoring heuristics.
 * Crucial for SIMD helpers where the call overhead exceeds the computation
 * cost.
 */
#define UTILS_ALWAYS_INLINE __attribute__((always_inline)) inline

/**
 * @brief Grants the compiler permission to generate specific SIMD
 * instructions inside a specific function, even if they are disabled
 * globally in the build system. This prevents 'always_inline function
 * requires target feature' errors in GCC/Clang.
 */
#define UTILS_TARGET_AVX2 \
__attribute__ ((target("avx2,bmi,bmi2,lzcnt")))
#define UTILS_TARGET_AVX512 \
__attribute__ ((target("avx512f,avx512bw,avx512vl")))

#elif defined(_MSC_VER)
// MSVC uses a specific keyword for forced inlining.
#define UTILS_ALWAYS_INLINE __forceinline

// MSVC allows the use of intrinsics without explicit target attributes,
// relying on the programmer to ensure runtime safety via CPU dispatching.
#define UTILS_TARGET_AVX2
#define UTILS_TARGET_AVX512
#else
// Fallback layer for non-x86 architectures (e.g., ARM64 / Apple Silicon).
#define UTILS_ALWAYS_INLINE inline
#define UTILS_TARGET_AVX2
#define UTILS_TARGET_AVX512
#endif

namespace utils::platform::simd
{

} // namespace utils::platform::simd
