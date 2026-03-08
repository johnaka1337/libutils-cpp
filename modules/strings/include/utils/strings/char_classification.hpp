#pragma once

#include "utils/strings/concepts.hpp"
#include "utils/platform/types.hpp"
#include "utils/platform/compiler.hpp"

#include <locale>




namespace utils::strings
{
inline namespace classification
{
using uint32_t = platform::uint32_t;
// 41h = 'A'
// 5Ah = 'Z'
// 61h = 'a'
// 7Ah = 'z'

// =========================================================================
// FAST PATH (ASCII & Unicode Branchless)
// =========================================================================

/**
 * @brief Checks if a character is a lowercase alphabetic letter
 * (ASCII only).
 *
 * @details Executes in a branchless manner using 32-bit integer arithmetic.
 * It strictly checks the ASCII range [0x61, 0x7A] ('a' - 'z').
 *
 * @tparam T The character type (satisfies CharType concept).
 * @param ch The character to check.
 * @return true if the character is a lowercase ASCII letter, false
 * otherwise.
 */
template< CharType T >
[[nodiscard]] constexpr bool IsLower( T ch ) noexcept
{
    const auto ch32 = static_cast< uint32_t >( ch );
    return ( ch32 - 0x61 ) <= ( 0x7A - 0x61 );
}



/**
 * @brief Checks if a character is an uppercase alphabetic letter
 * (ASCII only).
 *
 * @details Executes in a branchless manner using 32-bit integer arithmetic.
 * It strictly checks the ASCII range [0x41, 0x5A] ('A' - 'Z').
 *
 * @tparam T The character type (satisfies CharType concept).
 * @param ch The character to check.
 * @return true if the character is an uppercase ASCII letter, false
 * otherwise.
 */
template< CharType T >
[[nodiscard]] constexpr bool IsUpper( T ch ) noexcept
{
    const auto ch32 = static_cast< uint32_t >( ch );
    return ( ch32 - 0x41 ) <= ( 0x5A - 0x41 );
}



/**
 * @brief Checks if a character is an alphabetic letter (ASCII only).
 *
 * @details Uses a bitwise OR operation to normalize the character case
 * before checking the range, resulting in highly optimized branchless
 * execution.
 *
 * @tparam T The character type (satisfies CharType concept).
 * @param ch The character to check.
 * @return true if the character is an ASCII letter, false otherwise.
 */
template< CharType T >
[[nodiscard]] constexpr bool IsAlpha( T ch ) noexcept
{
    const auto ch32 = static_cast< uint32_t >( ch );
    return ( ( ch32 | 0x20 ) - 0x61 ) <= ( 0x7A - 0x61 );
}



/**
 * @brief Checks if a character is a whitespace character.
 *
 * @details Identifies standard ASCII whitespaces (space, tab, CR, LF, etc).
 * If the character type size is 2 bytes or larger (e.g., char16_t,
 * char32_t, wchar_t), it also checks for extended Unicode whitespace
 * characters (e.g., NBSP, Ideographic Space) using compile-time evaluation
 * (`if constexpr`).
 *
 * @tparam T The character type (satisfies CharType concept).
 * @param ch The character to check.
 * @return true if the character is a whitespace, false otherwise.
 */
template< CharType T >
[[nodiscard]] constexpr bool IsSpace( T ch ) noexcept
{
    const auto ch32 = static_cast< uint32_t >( ch );

    bool result = ( ch32 == 0x20 ) || ( ( ch32 - 0x09 ) <= ( 0x0D - 0x09 ) );

    if constexpr( sizeof( T ) >= 2 )
    {
        result = result ||
                 ch32 == 0x00A0 || // NBSP (Non-breaking space)
                 (ch32 - 0x2000) <= (0x200A - 0x2000) || // En, Em, ...
                 ch32 == 0x2028 || // Line Separator
                 ch32 == 0x2029 || // Paragraph Separator
                 ch32 == 0x202F || // Narrow NBSP
                 ch32 == 0x205F || // Medium Mathematical Space
                 ch32 == 0x3000 || // Ideographic Space
                 ch32 == 0x1680;   // Ogham Space Mark
    }

    return result;
}



// =========================================================================
// SLOW PATH (OS specific)
// =========================================================================

/**
 * @brief Checks if a legacy character is lowercase using the specified
 * locale.
 *
 * @details This is the slow path that uses `std::islower`. It provides a
 * strict `noexcept` guarantee. If exceptions are disabled via compiler
 * flags, or if the locale throws an exception during evaluation (e.g.,
 * `std::bad_cast`), it gracefully falls back to the branchless ASCII
 * implementation.
 *
 * @tparam T The character type (restricted to legacy types).
 * @param ch The character to check.
 * @param loc The `std::locale` to use for classification.
 * @return true if the character is lowercase according to the locale or
 * fallback.
 */
template< details::LegacyCharType T >
[[nodiscard]]
inline bool IsLower( T ch, const std::locale& loc ) noexcept
{
#if UTILS_EXCEPTIONS_ENABLED
    try
    {
        return std::islower( ch, loc );
    }
    catch ( ... )
    {
        return IsLower( ch );
    }
#else
    (void)loc;
    return IsLower( ch );
#endif
}



/**
 * @brief Checks if a legacy character is uppercase using the specified
 * locale.
 *
 * @details Uses `std::isupper` with a safe, exception-free fallback to the
 * ASCII fast path.
 *
 * @tparam T The character type (restricted to legacy types).
 * @param ch The character to check.
 * @param loc The `std::locale` to use for classification.
 * @return true if the character is uppercase according to the locale or
 * fallback.
 */
template< details::LegacyCharType T >
[[nodiscard]]
inline bool IsUpper( T ch, const std::locale& loc ) noexcept
{
#if UTILS_EXCEPTIONS_ENABLED
    try
    {
        return std::isupper( ch, loc );
    }
    catch ( ... )
    {
        return IsUpper( ch );
    }
#else
    (void)loc;
    return IsUpper( ch );
#endif
}



/**
 * @brief Checks if a legacy character is an alphabetic letter using the
 * specified locale.
 *
 * @details Uses `std::isalpha` with a safe, exception-free fallback to the
 * ASCII fast path.
 *
 * @tparam T The character type (restricted to legacy types).
 * @param ch The character to check.
 * @param loc The `std::locale` to use for classification.
 * @return true if the character is alphabetic according to the locale or
 * fallback.
 */
template< details::LegacyCharType T >
[[nodiscard]]
inline bool IsAlpha( T ch, const std::locale& loc ) noexcept
{
#if UTILS_EXCEPTIONS_ENABLED
    try
    {
        return std::isalpha( ch, loc );
    }
    catch ( ... )
    {
        return IsAlpha( ch );
    }
#else
    (void)loc;
    return IsAlpha( ch );
#endif
}

/**
 * @brief Checks if a legacy character is a whitespace using the specified
 * locale.
 *
 * @details Uses `std::isspace` with a safe, exception-free fallback to
 * the ASCII fast path.
 *
 * @tparam T The character type (restricted to legacy types).
 * @param ch The character to check.
 * @param loc The `std::locale` to use for classification.
 * @return true if the character is a whitespace according to the locale or
 * fallback.
 */
template< details::LegacyCharType T >
[[nodiscard]]
inline bool IsSpace( T ch, const std::locale& loc ) noexcept
{
#if UTILS_EXCEPTIONS_ENABLED
    try
    {
        return std::isspace( ch, loc );
    }
    catch ( ... )
    {
        return IsSpace( ch );
    }
#else
    (void)loc;
    return IsSpace( ch );
#endif
}
} // namespace classification
} // namespace utils::strings
