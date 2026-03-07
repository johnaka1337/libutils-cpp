#pragma once

// =========================================================================
// Compiler Feature Detection
// =========================================================================

/**
 * @def UTILS_EXCEPTIONS_ENABLED
 *
 * @brief Defined as 1 if the compiler supports C++ exceptions, otherwise 0.
 *
 * @details This macro checks standard compiler definitions to determine if
 * exception handling (stack unwinding) is enabled. It evaluates to 0 when
 * the project is compiled with flags such as `-fno-exceptions` in GCC/Clang.
 */
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
# define UTILS_EXCEPTIONS_ENABLED 1
#else
# define UTILS_EXCEPTIONS_ENABLED 0
#endif



/**
 * @def UTILS_RTTI_ENABLED
 *
 * @brief Defined as 1 if the compiler supports Run-Time Type Information
 * (RTTI), otherwise 0.
 *
 * @details This macro checks if RTTI is available for features like
 * `dynamic_cast` and `typeid`. It evaluates to 0 when compiled with flags
 * such as `-fno-rtti`.
 */
#if defined(__cpp_rtti) || defined(__GXX_RTTI) || defined(_CPPRTTI)
#define UTILS_RTTI_ENABLED 1
#else
#define UTILS_RTTI_ENABLED 0
#endif
