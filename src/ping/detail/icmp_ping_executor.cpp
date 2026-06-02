#include <system_error>
#include <iostream>
#include <chrono>
#include <format>
#include <string>
#include <thread>

#include "net/ipv4/icmp/header.hpp"

#include "net/ipv4/endpoint.hpp"

#include "ping/detail/icmp_ping_executor.hpp"
#include "ping/detail/timer.hpp"

void ping::detail::icmp_ping_executor::execute()
{
    std::string buffer;

    net::ipv4::endpoint source_endpoint;

    timer timer {
        [&](std::error_code& error)
        {
            socket_.receive_from(error, source_endpoint, buffer);
        }
    };

    socket_.open();

    std::cout << std::format("ping {} with {}({}) bytes of data\n\n",
        destination_.address(),
        message_.size(),
        message_.size() + net::ipv4::icmp::header::echo_message_header_size);

    for (; count_; --count_)
    {
        ping_once();

        const auto begin = std::chrono::steady_clock::now();

        timer.start_countdown(timeout_);

        const auto end = std::chrono::steady_clock::now();

        if (not timer.is_expired())
        {
            if (verify_response(source_endpoint, buffer))
            {
                [[maybe_unused]] const auto response_time =
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - begin);
            }
        }

        std::this_thread::sleep_for(interval_);
    }
}

void ping::detail::icmp_ping_executor::ping_once()
{
    socket_.send_to(
        destination_, net::ipv4::icmp::make_icmp_message(header_, message_));

    ++header_.echo_message.sequence_number;

    ++statistics_.sent_packets;

    std::cout << std::format(
        "sent {} bytes to {}: identifier={} sequence_number={}\n",
        message_.size() + net::ipv4::icmp::header::echo_message_header_size,
        destination_.address(),
        header_.echo_message.identifier,
        header_.echo_message.sequence_number);
}

bool ping::detail::icmp_ping_executor::verify_response(
    const net::ipv4::endpoint& source_endpoint, std::string_view data)
{
    if (destination_.address() == source_endpoint.address())
    {
        const auto& [header, message] = net::ipv4::icmp::unpack_icmp_message(
            net::ipv4::icmp::header::type_enumerator::echo_reply, data);

        if (header_ == header && message_ == message)
        {
            ++statistics_.received_packets;

            std::cout << std::format(
                "received {} bytes from {}: "
                "identifier={} sequence_number={}\n",
                data.size(),
                source_endpoint.address(),
                header.echo_message.identifier,
                header.echo_message.sequence_number);

            return true;
        }
    }

    return false;
}
