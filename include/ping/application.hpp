#pragma once

#include <string_view>
#include <chrono>

#include <cstdint>

namespace ping
{
    class application final
    {
    public:

        struct configuration final
        {
            std::string_view     address;
            std::string_view     icmp_echo_message;
            std::uint16_t        icmp_echo_message_identifier;
            std::chrono::seconds response_timeout;
        };

        struct statistics final
        {
            std::uint16_t received_packets;
            std::uint16_t sent_packets;
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

    public:

        configuration configuration_;
    };
}
