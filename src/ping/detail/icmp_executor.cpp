#include <system_error>
#include <iostream>
#include <chrono>
#include <format>
#include <string>
#include <thread>

#include "net/ipv4/icmp/icmp.hpp"

#include "net/ipv4/endpoint.hpp"

#include "ping/detail/icmp_executor.hpp"
#include "ping/detail/timer.hpp"

#include "ping/statistics.hpp"

namespace
{
    struct execution_context final
    {
        execution_context(const ping::configuration& configuration) :
            header {
                .type = net::ipv4::icmp::header::type_enumerator::echo,
                .code = 0,

                .echo_message {
                    .identifier      = configuration.identifier,
                    .sequence_number = 0
                }
            },

            socket {
                net::ipv4::endpoint {configuration.address}
            },

            timer {
                [this](std::error_code& error)
                {
                    socket.receive(error, buffer);
                }
            }
        {
            buffer.reserve(512);
        }

        std::string             buffer;
        net::ipv4::icmp::header header;
        net::ipv4::icmp::socket socket;
        ping::detail::timer     timer;
    };

    void check_echo_reply(
        const ping::configuration& configuration,
        execution_context&         execution_context,
        ping::statistics&          statistics)
    {
        if (execution_context.timer.is_expired())
        {
            std::cout << "\n\n";
        }

        else
        {
            execution_context.timer.cancel();

            const auto icmp_packet = net::ipv4::icmp::header::from_data(
                execution_context.buffer);

            if (icmp_packet.has_value())
            {
                const auto& [icmp_header, icmp_data] = icmp_packet.value();

                if (icmp_header.type ==
                        net::ipv4::icmp::header::type_enumerator::echo_reply &&
                    icmp_header.code ==
                        execution_context.header.code &&
                    icmp_header.echo_message.identifier ==
                        execution_context.header.echo_message.identifier &&
                    icmp_header.echo_message.sequence_number ==
                        execution_context.header.echo_message.sequence_number)
                {
                    ++statistics.received_packets;

                    std::cout << std::format(
                        "received {} bytes from {}: "
                        ".identifier={} .sequence_number={}\n",
                        execution_context.header.echo_message_header_size +
                            icmp_data.size(),
                        configuration.address,
                        icmp_header.echo_message.identifier,
                        icmp_header.echo_message.sequence_number
                    );
                }
            }
        }
    }

    void send_echo_request(
        const ping::configuration& configuration,
        execution_context&         execution_context,
        ping::statistics&          statistics)
    {
        const auto icmp_echo_message = net::ipv4::icmp::make_icmp_message(
            execution_context.header, configuration.message);

        execution_context.socket.send(icmp_echo_message);

        ++statistics.sent_packets;

        std::cout << std::format(
            "sent {} bytes to {}: .identifier={} .sequence_number={}\n",
            icmp_echo_message.size(),
            configuration.address,
            execution_context.header.echo_message.identifier,
            execution_context.header.echo_message.sequence_number
        );
    }
}

void ping::detail::icmp_executor::execute()
{
    execution_context execution_context {configuration_};

    for (int counter = 0; ; )
    {
        send_echo_request(configuration_, execution_context, statistics_);

        execution_context.timer.start_countdown(configuration_.timeout);

        check_echo_reply(configuration_, execution_context, statistics_);

        if (++counter >= configuration_.count)
        {
            break;
        }

        ++execution_context.header.echo_message.sequence_number;

        std::this_thread::sleep_for(configuration_.interval);
    }
}
