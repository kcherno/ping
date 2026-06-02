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

                destination_endpoint_ {
                    configuration_.address
                },

                header_ {
                    .type     = net::ipv4::icmp::header::type_enumerator::echo,
                    .code     = 0,
                    .checksum = 0,

                    .echo_message {
                        .identifier      = configuration_.identifier,
                        .sequence_number = 0
                    }
                },

                message_ {
                    configuration.icmp_echo_message
                },

                response_timeout_ {
                    configuration.response_timeout
                },

                statistics_ {
                    statistics
                }
        {}

        void execute() override;

    private:

        void ping_once();

        bool verify_response(const net::ipv4::endpoint&, std::string_view);

        std::uint16_t            count_;
        net::ipv4::endpoint      destination_endpoint_;
        net::ipv4::icmp::header  header_;
        std::string_view         message_;
        std::chrono::seconds     response_timeout_;
        net::ipv4::icmp::socket  socket_;
        application::statistics& statistics_;
    };
}
