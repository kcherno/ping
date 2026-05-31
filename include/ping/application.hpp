#pragma once

#include <string_view>

#include <cstdint>

namespace ping
{
    class application final
    {
    public:

        struct configuration final
        {
            std::string_view icmp_echo_message;
            std::uint16_t    icmp_echo_message_identifier;
        };

        struct statistics final
        {

        };

        application(int argc, char** argv)
        {

        }

        static consteval std::string_view name() noexcept
        {
            return "ping";
        }

        void run() const
        {

        }
    };
}
