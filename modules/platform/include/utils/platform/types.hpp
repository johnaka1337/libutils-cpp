#pragma once

#include <cstddef>
#include <cstdint>
#include <bit>




namespace utils::platform
{
using Byte = std::byte;

using Int8  = std::int8_t;
using UInt8 = std::uint8_t;

using Int16  = std::int16_t;
using UInt16 = std::uint16_t;

using Int32  = std::int32_t;
using UInt32 = std::uint32_t;

using Int64  = std::int64_t;
using UInt64 = std::uint64_t;

using Size = std::size_t;



constexpr bool IsLittleEndian( ) noexcept
{
    return (std::endian::native == std::endian::little);
}


constexpr bool IsBigEndian( ) noexcept
{
    return (std::endian::native == std::endian::big);
}
} // namespace utils::platform
