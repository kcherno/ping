#include <system_error>
#include <string_view>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <limits>

#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include "ping.hpp"

namespace
{
    auto statistics = ping::analysis::statistics {
	.sent_packets     = 0,
	.received_packets = 0
    };
}

template<typename ExecutionContext>
void do_ping(ExecutionContext& executor, std::string_view address)
{
    auto error = boost::system::error_code {};

    const auto destination_address =
	boost::asio::ip::make_address(address, error);

    if (error)
    {
	throw boost::system::system_error {error, "make_address"};
    }

    auto socket = boost::asio::ip::icmp::socket {
	executor,
	destination_address.is_v4() ?
	boost::asio::ip::icmp::v4() :
	boost::asio::ip::icmp::v6()
    };

    auto endpoint = boost::asio::ip::icmp::endpoint {
	destination_address,
	0
    };

    std::array<uint8_t, 32> buffer;

    const auto sleep_duration = ::timespec {
	.tv_sec  = 1,
	.tv_nsec = 0
    };

    uint16_t sequence_number = 0;

    constexpr auto max_sequence_number =
	std::numeric_limits<uint16_t>::max();

    for (; sequence_number < max_sequence_number; ++sequence_number)
    {
	auto& mutable_icmp_echo =
	    *reinterpret_cast<ping::icmp::icmp_echo_header*>(buffer.data());

	mutable_icmp_echo.type(8);
	mutable_icmp_echo.code(0);
	mutable_icmp_echo.identifier(0);
	mutable_icmp_echo.sequence_number(sequence_number);
	mutable_icmp_echo.calculate_checksum(mutable_icmp_echo.header_length());

	const auto const_buffer =
	    boost::asio::buffer(buffer.data(), mutable_icmp_echo.header_length());

	socket.send_to(const_buffer, endpoint);

	++statistics.sent_packets;

	const auto received_bytes =
	    socket.receive_from(boost::asio::buffer(buffer), endpoint);

	++statistics.received_packets;

	const auto& const_ip_header =
	    *reinterpret_cast<const ping::ip::ipv4_header*>(buffer.data());

	const auto& const_icmp_echo =
	    *reinterpret_cast<const ping::icmp::icmp_echo_header*>(const_ip_header.data());

	std::cout << received_bytes
		  << " received bytes from "
		  << endpoint.address()
		  << ": "
		  << "icmp_sequence_number="
		  << const_icmp_echo.sequence_number()
		  << " ttl="
		  << static_cast<int>(const_ip_header.time_to_live())
		  << std::endl;

	::nanosleep(&sleep_duration, nullptr);
    }
}

void terminal_interrupt_handler(int)
{
    std::cout << statistics << std::endl;

    std::exit(EXIT_SUCCESS);
}

void set_signal_handler(int signal, struct ::sigaction& signal_action)
{
    if (::sigaction(signal, &signal_action, nullptr) == -1)
    {
	auto error = std::make_error_code(std::errc {errno});

	throw std::system_error {error, __func__};
    }
}

int main(int argc, char** argv)
{
    try
    {
	if (argc == 1)
	{
	    throw std::runtime_error {"Usage: ping -h|--help"};
	}

	else
	{
	    auto option = std::string_view(argv[1]);

	    if (option == "-h" || option == "--help")
	    {
		std::cout << "Usage: ping <address>" << std::endl;

		return 0;
	    }
	}

	struct ::sigaction interrupt_signal;

	interrupt_signal.sa_handler = &terminal_interrupt_handler;

	auto quit_signal = interrupt_signal;

	set_signal_handler(SIGINT,  interrupt_signal);
	set_signal_handler(SIGQUIT, quit_signal);

	auto io_context = boost::asio::io_context {};

	do_ping(io_context, argv[1]);
    }

    catch (const std::exception& e)
    {
	std::cerr << e.what() << std::endl;

	return 1;
    }
}
