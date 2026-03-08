#pragma once

#include <concepts>
#include <type_traits>
#include <string>
#include <string_view>




namespace utils::strings
{
using std::same_as;
using std::remove_cvref_t;



namespace details
{
/**
 * @brief Concept that checks if a type is a traditional C/C++ character
 * type.
 *
 * @details This matches `char` (the default system-dependent character
 * type, often used for ASCII or UTF-8) and `wchar_t` (the legacy wide
 * character type, prominently used in the Windows API).
 * Strips `const`, `volatile`, and reference qualifiers before evaluation.
 *
 * @tparam T The type to be evaluated.
 */
template< typename T >
concept LegacyCharType = same_as< remove_cvref_t< T >, char > ||
                         same_as< remove_cvref_t< T >, wchar_t >;


/**
 * @brief Concept that checks if a type is a Unicode-aware character type.
 *
 * @details This matches the modern explicit Unicode types introduced in
 * C++11 and C++20 (`char8_t`, `char16_t`, `char32_t`), as well as `wchar_t`.
 * Strips `const`, `volatile`, and reference qualifiers before evaluation.
 *
 * @tparam T The type to be evaluated.
 */
template< typename T >
concept UnicodeCharType = same_as< remove_cvref_t< T >, wchar_t >  ||
                          same_as< remove_cvref_t< T >, char8_t >  ||
                          same_as< remove_cvref_t< T >, char16_t > ||
                          same_as< remove_cvref_t< T >, char32_t >;
} // namespace details



/**
 * @brief A universal concept that satisfies any standard C++ character
 * type.
 *
 * @details Combines `LegacyCharType` and `UnicodeCharType`. This is the
 * primary constraint to be used in string manipulation functions
 * (e.g., split, trim, join) to ensure the template arguments are strictly
 * valid characters, preventing unintended instantiations with integers or
 * custom types.
 *
 * @tparam T The type to be evaluated.
 */
template< typename T >
concept CharType = details::LegacyCharType< T > ||
                   details::UnicodeCharType< T >;



/**
 * @brief Matches owning string types (std::string, std::wstring, etc).
 *
 * @details Strictly checks the base type without references or const
 * qualifiers. Used for in-place mutating algorithms.
 */
template< typename T >
concept StringType =
    same_as< remove_cvref_t< T >, std::string > ||
    same_as< remove_cvref_t< T >, std::wstring > ||
    same_as< remove_cvref_t< T >, std::u8string > ||
    same_as< remove_cvref_t< T >, std::u16string > ||
    same_as< remove_cvref_t< T >, std::u32string >;



/**
 * @brief Matches anything convertible to a string view.
 *
 * @details Supports C-strings (const char*), string literals ("text"),
 * std::string, and std::string_view itself via C++17 CTAD.
 * Used for read-only, zero-allocation algorithms.
 */
template< typename T >
concept ViewType = requires( const remove_cvref_t< T >& str )
{
    std::basic_string_view{ str };
};
} // namespace utils::strings
