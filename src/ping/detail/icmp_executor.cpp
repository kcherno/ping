#include <system_error>
#include <iostream>
#include <chrono>
#include <format>
#include <string>
#include <thread>

#include "net/ipv4/icmp/icmp.hpp"

#include "ping/detail/icmp_executor.hpp"
#include "ping/detail/timer.hpp"

namespace
{
    struct execution_context final
    {
        execution_context(const ping::configuration& configuration) :
            destionation {
                configuration.destination
            },

            header {
                .type = net::ipv4::icmp::header::type_enumerator::echo,
                .code = 0,

                .echo_message {
                    .identifier      = configuration.identifier,
                    .sequence_number = 0
                }
            }
        {
            buffer.reserve(512);

            socket.open();

            timer = ping::detail::timer {
                [&buffer, &socket, &source](std::error_code& error)
                {
                    socket.receive_from(error, source, buffer);
                }
            };
        }

        std::string             buffer;
        net::ipv4::endpoint     destionation;
        net::ipv4::icmp::header header;
        net::ipv4::icmp::socket socket;
        net::ipv4::endpoint     source;
        ping::detail::timer     timer;
    };

    inline constexpr bool compare_icmp_headers(
        const net::ipv4::icmp::header& echo,
        const net::ipv4::icmp::header& reply)
    {
        using enum net::ipv4::icmp::header::type_enumerator;

        return (reply.type ==
                    echo_reply &&
                reply.code ==
                    echo.code &&
                reply.echo_message.identifier ==
                    echo.echo_message.identifier &&
                reply.echo_message.sequence_number ==
                    echo.echo_message.sequence_number);
    }

    void check_echo_reply(
        const ping::configuration&     configuration,
        execution_context&             execution_context,
        ping::application::statistics& statistics)
    {
        if (execution_context.timer.is_expired())
        {
            std::cout << std::endl;
        }

        else
        {
            const auto icmp_packet = net::ipv4::icmp::header::from_data(
                execution_context.buffer
            );

            if (icmp_packet.has_value())
            {
                const auto& [icmp_header, icmp_data] = icmp_packet.value();

                if (compare_icmp_headers(
                        execution_context.header, icmp_header))
                {
                    ++statistics.received_packets;

                    std::cout << std::format(
                        "received {} bytes from {}:"
                        ".identifier={} .seuquence_number={}\n",
                        execution_context.buffer.size(),
                        execution_context.source.address(),
                        icmp_header.echo_message.identifier,
                        icmp_header.echo_message.sequence_number
                    );
                }
            }
        }
    }

    void send_echo_request(
        const ping::configuration&     configuration,
        execution_context&             execution_context,
        ping::application::statistics& statistics)
    {
        const auto icmp_echo_message = net::ipv4::icmp::make_icmp_message(
            execution_context.header, configuration.message
        );

        execution_context.socket.send_to(
            execution_context.destination,
            icmp_echo_message
        );

        ++statistics.sent_packets;

        std::cout << std::format(
            "sent {} bytes to {}: .identifier={} .sequence_number={}\n"
            icmp_echo_message.size(),
            execution_context.destination.address(),
            execution_context.header.echo_message.identifier,
            execution_context.header.echo_message.sequence_number
        );
    }
}

void ping::detail::icmp_executor::execute()
{
    execution_context execution_context {configuration_};

    for (int counter = 0; counter <= configuration_.count(); ++count)
    {
        send_echo_request(configuration_, execution_context, statistics_);

        execution_context.timer.start_countdown(configuration_.timeout());

        check_echo_reply(configuration_, execution_context, statistics_);

        std::this_thread::sleep_for(configuration_.interval());
    }
}
