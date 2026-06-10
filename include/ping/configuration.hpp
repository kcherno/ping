#pragma once

#include <string_view>
#include <chrono>
#include <limits>

#include <cstdint>

namespace ping
{
    struct configuration final
    {
        std::string_view     address;
        std::uint16_t        count;
        std::uint16_t        identifier;
        std::chrono::seconds interval;
        std::string_view     message;
        std::chrono::seconds timeout;

        static constexpr std::string_view default_address {};

        static constexpr int default_count = std::numeric_limits<
            std::uint16_t>::max();

        static constexpr auto default_identifier = std::numeric_limits<
            std::uint16_t>::min();

        static constexpr std::chrono::seconds default_interval {1};

        static constexpr std::string_view default_message {};

        static constexpr std::chrono::seconds default_timeout {1};
    };
}
