#pragma once

#include "utils/strings/concepts.hpp"
#include "utils/strings/char_transform.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <locale>
#include <version>




namespace utils::strings
{
inline namespace transform
{
// =========================================================================
// READ-ONLY PATH (Allocates new std::string)
// =========================================================================

/**
 * @brief Converts a string-like object to lowercase (ASCII fast-path).
 *
 * @details Takes any string-like object (satisfying ViewType) and returns
 * a newly allocated std::basic_string with all ASCII letters converted to
 * lowercase. Automatically utilizes C++23 `resize_and_overwrite` if
 * available, otherwise falls back to C++20 `reserve` + `push_back`.
 *
 * @tparam T The type of the input string.
 * @param str The string-like object to evaluate.
 * @return A new string with the transformed characters.
 */
template< ViewType T >
[[nodiscard]]
constexpr auto ToLower( const T& str )
{
    auto sv = std::basic_string_view{ str };
    std::basic_string< CharTypeOf< T > > result;

#if defined(__cpp_lib_string_resize_and_overwrite)
    // C++23 Fast Path
    result.resize_and_overwrite( sv.length(), [&]( auto* buf, auto n ) {
        for ( decltype(n) i = 0; i < n; ++i )
        {
            buf[ i ] = ToLower( sv[ i ] );
        }
        return n;
    } );
#else
    // C++20 Fallback
    result.reserve( sv.length() );
    for( const auto ch : sv )
    {
        result.push_back( ToLower( ch ) );
    }
#endif

    return result;
}


/**
 * @brief Converts a string-like object to uppercase (ASCII fast-path).
 *
 * @details Takes any string-like object (satisfying ViewType) and returns
 * a newly allocated std::basic_string with all ASCII letters converted to
 * uppercase. Automatically utilizes C++23 `resize_and_overwrite` if
 * available.
 *
 * @tparam T The type of the input string.
 * @param str The string-like object to evaluate.
 * @return A new string with the transformed characters.
 */
template< ViewType T >
[[nodiscard]]
constexpr auto ToUpper( const T& str )
{
    auto sv = std::basic_string_view{ str };
    std::basic_string< CharTypeOf< T > > result;

#if defined(__cpp_lib_string_resize_and_overwrite)
    // C++23 Fast Path
    result.resize_and_overwrite( sv.length(), [&]( auto* buf, auto n ) {
        for ( decltype(n) i = 0; i < n; ++i )
        {
            buf[ i ] = ToUpper( sv[ i ] );
        }
        return n;
    } );
#else
    // C++20 Fallback
    result.reserve( sv.length() );
    for( const auto ch : sv )
    {
        result.push_back( ToUpper( ch ) );
    }
#endif

    return result;
}


/**
 * @brief Converts a legacy string to lowercase using a specific locale.
 *
 * @details Only participates in overload resolution if the string contains
 * legacy character types (char or wchar_t). Uses C++23
 * `resize_and_overwrite` if supported by the compiler, otherwise falls
 * back to C++20 allocation.
 *
 * @tparam T The type of the input string (must contain legacy characters).
 * @param str The string-like object to evaluate.
 * @param loc The `std::locale` to use for transformation.
 * @return A new string with the transformed characters according to the
 * locale.
 */
template< ViewType T >
    requires details::LegacyCharType< CharTypeOf< T > >
[[nodiscard]]
inline auto ToLower( const T& str, const std::locale& loc )
{
    auto sv = std::basic_string_view{ str };
    std::basic_string< CharTypeOf< T > > result;

#if defined(__cpp_lib_string_resize_and_overwrite)
    // C++23 Fast Path
    result.resize_and_overwrite( sv.length(), [&]( auto* buf, auto n ) {
        for ( decltype(n) i = 0; i < n; ++i )
        {
            buf[ i ] = ToLower( sv[ i ], loc );
        }
        return n;
    } );
#else
    // C++20 Fallback
    result.reserve( sv.length() );
    for( const auto ch : sv )
    {
        result.push_back( ToLower( ch, loc ) );
    }
#endif

    return result;
}


/**
 * @brief Converts a legacy string to uppercase using a specific locale.
 *
 * @details Only participates in overload resolution if the string contains
 * legacy character types (char or wchar_t).
 *
 * @tparam T The type of the input string (must contain legacy characters).
 * @param str The string-like object to evaluate.
 * @param loc The `std::locale` to use for transformation.
 * @return A new string with the transformed characters according to the
 * locale.
 */
template< ViewType T >
    requires details::LegacyCharType< CharTypeOf< T > >
[[nodiscard]] inline auto ToUpper( const T& str, const std::locale& loc )
{
    auto sv = std::basic_string_view{ str };
    std::basic_string< CharTypeOf< T > > result;

#if defined(__cpp_lib_string_resize_and_overwrite)
    // C++23 Fast Path
    result.resize_and_overwrite( sv.length(), [&]( auto* buf, auto n ) {
        for ( decltype(n) i = 0; i < n; ++i )
        {
            buf[ i ] = ToUpper( sv[ i ], loc );
        }
        return n;
    } );
#else
    // C++20 Fallback
    result.reserve( sv.length() );
    for( const auto ch : sv )
    {
        result.push_back( ToUpper( ch, loc ) );
    }
#endif

    return result;
}



// =========================================================================
// IN-PLACE MUTATION PATH (Perfect Forwarding)
// =========================================================================

/**
 * @brief Converts a string to lowercase in-place (ASCII fast-path).
 *
 * @details Modifies the original string buffer by iterating through it.
 * Exceptionally fast and allocates absolutely zero memory.
 * Uses universal references to allow method chaining.
 *
 * @tparam T The string type (satisfies StringType).
 * @param str The owning string to modify.
 * @return A forwarded reference to the modified string.
 */
template< StringType T >
constexpr T&& ToLowerInPlace( T&& str ) noexcept
{
    static_assert( ! std::is_const_v< std::remove_reference_t< T > >,
                  "const mutation!" );

    for( auto& ch : str )
    {
        ch = ToLower( ch );
    }

    return std::forward< T >( str );
}


/**
 * @brief Converts a string to uppercase in-place (ASCII fast-path).
 *
 * @details Modifies the original string buffer without allocations.
 * Safe for method chaining.
 *
 * @tparam T The string type (satisfies StringType).
 * @param str The owning string to modify.
 * @return A forwarded reference to the modified string.
 */
template< StringType T >
constexpr T&& ToUpperInPlace( T&& str ) noexcept
{
    static_assert( ! std::is_const_v< std::remove_reference_t< T > >,
                  "const mutation" );

    for( auto& ch : str )
    {
        ch = ToUpper( ch );
    }

    return std::forward< T >( str );
}


/**
 * @brief Converts a legacy string to lowercase in-place using a locale.
 *
 * @details Performs OS-level, locale-aware character transformation
 * directly within the existing string buffer. SFINAE-constrained to legacy
 * char types.
 *
 * @tparam T The string type (satisfies StringType and uses LegacyCharType).
 * @param str The owning string to modify.
 * @param loc The `std::locale` to use for transformation.
 * @return A forwarded reference to the modified string.
 */
template< StringType T >
    requires details::LegacyCharType< CharTypeOf< T > >
inline T&& ToLowerInPlace( T&& str, const std::locale& loc ) noexcept
{
    static_assert( ! std::is_const_v< std::remove_reference_t< T > >,
                  "const mutation" );

    for( auto& ch : str )
    {
        ch = ToLower( ch, loc );
    }

    return std::forward< T >( str );
}


/**
 * @brief Converts a legacy string to uppercase in-place using a locale.
 *
 * @details Performs OS-level, locale-aware character transformation
 * directly within the existing string buffer.
 *
 * @tparam T The string type (satisfies StringType and uses LegacyCharType).
 * @param str The owning string to modify.
 * @param loc The `std::locale` to use for transformation.
 * @return A forwarded reference to the modified string.
 */
template< StringType T >
    requires details::LegacyCharType< CharTypeOf< T > >
inline T&& ToUpperInPlace( T&& str, const std::locale& loc ) noexcept
{
    static_assert( ! std::is_const_v< std::remove_reference_t< T > >,
                  "const mutation" );

    for( auto& ch : str )
    {
        ch = ToUpper( ch, loc );
    }

    return std::forward< T >( str );
}
} // namespace transform
} // namespace utils::strings
