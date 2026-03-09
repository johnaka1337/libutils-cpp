#pragma once

#include "utils/strings/concepts.hpp"
#include "utils/strings/char_classification.hpp"

#include <string_view>
#include <type_traits>
#include <utility>




namespace utils::strings
{
inline namespace transform
{
using size_t = platform::size_t;



// =========================================================================
// READ-ONLY PATH (Zero-Allocation)
// =========================================================================

/**
 * @brief Removes leading whitespace without mutating the original string.
 *
 * @details Takes any string-like object (literal, C-string, std::string,
 * std::string_view) and performs a zero-allocation operation by adjusting
 * the boundaries of a lightweight string view.
 *
 * @tparam T The type of the input string (satisfies ViewType).
 * @param str The string-like object to evaluate.
 * @return A lightweight std::basic_string_view of the trimmed data.
 */
template< ViewType T >
[[nodiscard]]
constexpr auto LTrim( const T& str ) noexcept
{
    auto sv = std::basic_string_view{ str };
    size_t i = 0;

    while( i < sv.length() && IsSpace( sv[ i ] ) )
    {
        ++i;
    }

    sv.remove_prefix( i );

    return sv;
}


/**
 * @brief Removes trailing whitespace without mutating the original string.
 *
 * @details Performs a zero-allocation operation by calculating the exact
 * amount of trailing whitespace and reducing the size of the view from the
 * back.
 *
 * @tparam T The type of the input string (satisfies ViewType).
 * @param input The string-like object to evaluate.
 * @return A lightweight std::basic_string_view of the trimmed data.
 */
template< ViewType T >
[[nodiscard]]
constexpr auto RTrim( const T& input ) noexcept
{
    auto sv = std::basic_string_view{ input };
    size_t i = sv.length();

    while( i > 0 && IsSpace( sv[ i - 1 ] ) )
    {
        --i;
    }

    sv.remove_suffix( sv.length() - i );

    return sv;
}


/**
 * @brief Removes both leading and trailing whitespace without mutating.
 *
 * @tparam T The type of the input string (satisfies ViewType).
 * @param str The string-like object to evaluate.
 * @return A lightweight std::basic_string_view with both ends trimmed.
 */
template< ViewType T >
[[nodiscard]]
constexpr auto Trim( const T& str ) noexcept
{
    return LTrim( RTrim( str ) );
}



// =========================================================================
// IN-PLACE MUTATION PATH (Perfect Forwarding)
// =========================================================================

/**
 * @brief Removes leading whitespace in-place.
 *
 * @details Uses universal references (T&&) to bind to both lvalues and
 * rvalues. Modifies the original string buffer by erasing characters from
 * the beginning. Safe for method chaining.
 *
 * @tparam T The string type (satisfies StringType).
 * @param str The owning string to modify.
 * @return A forwarded reference to the modified string.
 */
template< StringType T >
constexpr T&& LTrimInPlace( T&& str ) noexcept
{
    static_assert( ! std::is_const_v< std::remove_reference_t< T > >,
                  "const mutation!" );

    size_t start = 0;
    while ( start < str.length() && IsSpace( str[ start ] ) )
    {
        ++start;
    }

    if ( start > 0 )
    {
        str.erase( 0, start );
    }

    return std::forward< T >( str );
}


/**
 * @brief Removes trailing whitespace in-place.
 *
 * @details Modifies the original string buffer by truncating it from the
 * end. Extremely fast as it typically only changes the internal size
 * counter.
 *
 * @tparam T The string type (satisfies StringType).
 * @param str The owning string to modify.
 * @return A forwarded reference to the modified string.
 */
template< StringType T >
constexpr T&& RTrimInPlace( T&& str ) noexcept
{
    static_assert( ! std::is_const_v< std::remove_reference_t< T > >,
                  "const mutation" );

    size_t end = str.length();
    while ( end > 0 && IsSpace( str[ end - 1 ] ) )
    {
        --end;
    }

    if ( end < str.length() )
    {
        str.erase( end );
    }

    return std::forward< T >( str );
}


/**
 * @brief Removes both leading and trailing whitespace in-place.
 *
 * @tparam T The string type (satisfies StringType).
 * @param str The owning string to modify.
 * @return A forwarded reference to the modified string.
 */
template< StringType T >
constexpr T&& TrimInPlace( T&& str ) noexcept
{
    // RTrimInPlace is called first because erasing from the end is cheaper.
    // LTrimInPlace then shifts fewer bytes.
    return LTrimInPlace( RTrimInPlace( std::forward< T >( str ) ) );
}
} // namespace transform
} // namespace utils::strings
