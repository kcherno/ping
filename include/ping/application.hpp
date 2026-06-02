#pragma once

#include <string_view>
#include <chrono>
#include <memory>

#include <cstdint>

namespace ping
{
    class application final
    {
    public:

        class basic_application_executor
        {
        public:

            virtual ~basic_application_executor() = default;

            virtual void execute() = 0;
        };

        struct configuration final
        {
            std::uint16_t        count;
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
            executor_->execute();
        }

    public:

        configuration                               configuration_;
        statistics                                  statistics_;
        std::unique_ptr<basic_application_executor> executor_;
    };
}
