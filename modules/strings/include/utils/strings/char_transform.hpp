#pragma once

#include "utils/strings/concepts.hpp"
#include "utils/platform/types.hpp"
#include "utils/platform/compiler.hpp"

#include <locale>




namespace utils::strings
{
inline namespace transform
{
using uint32_t = platform::uint32_t;
// 41h = 'A'
// 5Ah = 'Z'
// 61h = 'a'
// 7Ah = 'z'

// =========================================================================
// FAST PATH (ASCII Branchless)
// =========================================================================

/**
 * @brief Converts an uppercase ASCII letter to lowercase.
 *
 * @details Executes in a purely branchless manner using bitwise operations.
 * It strictly checks if the character is in the [0x41, 0x5A] range.
 * If true, it sets the 6th bit (OR 0x20) to convert it to lowercase. Other
 * characters are returned unmodified.
 *
 * @tparam T The character type (satisfies CharType concept).
 * @param ch The character to convert.
 * @return The lowercase equivalent if it's an uppercase ASCII letter,
 * otherwise the original character.
 */
template< CharType T >
[[nodiscard]] constexpr T ToLower( T ch ) noexcept
{
    const auto ch32 = static_cast< uint32_t >( ch );

    // Explicitly cast the boolean evaluation to uint for strict type safety
    const auto is_upper =
        static_cast< uint32_t >( ( ch32 - 0x41 ) <= ( 0x5A - 0x41 ) );

    // If is_upper == 1, (1 << 5) == 0x20. OR with 0x20 makes it lowercase.
    // If is_upper == 0, (0 << 5) == 0. OR with 0 does nothing.
    return static_cast< T >( ch32 | ( is_upper << 5 ) );
}



/**
 * @brief Converts a lowercase ASCII letter to uppercase.
 *
 * @details Executes in a purely branchless manner using bitwise operations.
 * It strictly checks if the character is in the [0x61, 0x7A] range.
 * If true, it clears the 6th bit (XOR 0x20) to convert it to uppercase.
 * Other characters are returned unmodified.
 *
 * @tparam T The character type (satisfies CharType concept).
 * @param ch The character to convert.
 * @return The uppercase equivalent if it's a lowercase ASCII letter,
 * otherwise the original character.
 */
template< CharType T >
[[nodiscard]] constexpr T ToUpper( T ch ) noexcept
{
    const auto ch32 = static_cast< uint32_t >( ch );

    const auto is_lower =
        static_cast< uint32_t >( ( ch32 - 0x61 ) <= ( 0x7A - 0x61 ) );

    return static_cast< T >( ch32 ^ ( is_lower << 5 ) );
}



// =========================================================================
// SLOW PATH (OS specific)
// =========================================================================

/**
 * @brief Converts a legacy character to lowercase using the specified
 * locale.
 *
 * @details This is the slow path that uses `std::tolower`. It provides a
 * strict `noexcept` guarantee. If exceptions are disabled via compiler
 * flags, or if the locale throws an exception, it gracefully falls back
 * to the branchless ASCII implementation.
 *
 * @tparam T The character type (restricted to legacy types).
 * @param ch The character to convert.
 * @param loc The `std::locale` to use for transformation.
 * @return The lowercase equivalent according to the locale or fallback.
 */
template< details::LegacyCharType T >
[[nodiscard]]
inline T ToLower( T ch, const std::locale& loc ) noexcept
{
#if UTILS_EXCEPTIONS_ENABLED
    try
    {
        return std::tolower( ch, loc );
    }
    catch ( ... )
    {
        return ToLower( ch );
    }
#else
    (void)loc;
    return ToLower( ch );
#endif
}



/**
 * @brief Converts a legacy character to uppercase using the specified
 * locale.
 *
 * @details Uses `std::toupper` with a safe, exception-free fallback to the
 * ASCII fast path.
 *
 * @tparam T The character type (restricted to legacy types).
 * @param ch The character to convert.
 * @param loc The `std::locale` to use for transformation.
 * @return The uppercase equivalent according to the locale or fallback.
 */
template< details::LegacyCharType T >
[[nodiscard]]
inline T ToUpper( T ch, const std::locale& loc ) noexcept
{
#if UTILS_EXCEPTIONS_ENABLED
    try
    {
        return std::toupper( ch, loc );
    }
    catch ( ... )
    {
        return ToUpper( ch );
    }
#else
    (void)loc;
    return ToUpper( ch );
#endif
}
} // namespace transform
} // namespace utils::strings
