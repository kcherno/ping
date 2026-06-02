#pragma once

#include <string_view>
#include <chrono>

#include "net/ipv4/icmp/icmp.hpp"

#include "net/ipv4/endpoint.hpp"

#include "ping/application.hpp"

namespace ping::detail
{
    class icmp_ping_executor final :
        public application::basic_application_executor
    {
    public:

        icmp_ping_executor(
            const application::configuration& configuration,
            application::statistics&          statistics) :
                count_ {
                    configuration.count
                },

                destination_ {
                    configuration.destination
                },

                header_ {
                    .type     = net::ipv4::icmp::header::type_enumerator::echo,
                    .code     = 0,
                    .checksum = 0,

                    .echo_message {
                        .identifier      = configuration.identifier,
                        .sequence_number = 0
                    }
                },

                interval_ {
                    configuration.interval
                },

                message_ {
                    configuration.icmp_echo_message
                },

                statistics_ {
                    statistics
                },

                timeout_ {
                    configuration.timeout
                }
        {}

        void execute() override;

    private:

        void ping_once();

        bool verify_response(const net::ipv4::endpoint&, std::string_view);

        std::uint16_t            count_;
        net::ipv4::endpoint      destination_;
        net::ipv4::icmp::header  header_;
        std::chrono::seconds     interval_;
        std::string_view         message_;
        net::ipv4::icmp::socket  socket_;
        application::statistics& statistics_;
        std::chrono::seconds     timeout_;
    };
}
