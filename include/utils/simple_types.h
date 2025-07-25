#pragma once 

#include <cstdint>
#include <cstddef>

namespace rcp
{
    // basic type
    using byte = std::uint8_t;      // 8  bits unsigned
    using word = std::uint16_t;     // 16 bits unsigned
    using dword = std::uint32_t;    // 32 bits unsigned
    using ddword = std::uint64_t;   // 64 bits unsigned

    using ThreadID = std::size_t;
    using ProcessID = std::size_t;
    using WindowID = std::size_t;
    using MonitorIndex = std::uint64_t;

}
