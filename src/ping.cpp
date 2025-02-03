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

extern "C"
{

#include <unistd.h>

}

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

void do_ping(std::string_view address)
{
    auto error = boost::system::error_code {};

    const auto destination_endpoint = boost::asio::ip::icmp::endpoint {
	boost::asio::ip::make_address(address, error),
	0
    };

    if (error)
    {
	throw boost::system::system_error {error, "make_address"};
    }

    auto io_context = boost::asio::io_context {};

    auto socket = boost::asio::ip::icmp::socket {
	io_context,
	destination_endpoint.address().is_v4() ?
	boost::asio::ip::icmp::v4() :
	boost::asio::ip::icmp::v6()
    };

    std::cout << "PING "
	      << destination_endpoint.address()
	      << " 8 bytes of data"
	      << std::endl;

    std::array<uint8_t, 32> buffer;

    auto sequence_number = uint16_t {};

    while (sequence_number < std::numeric_limits<uint16_t>::max())
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

	socket.send_to(const_buffer, destination_endpoint);

	++statistics.sent_packets;

	auto sender_endpoint = boost::asio::ip::icmp::endpoint {};

	::alarm(1);

	const auto received_bytes =
	    socket.receive_from(boost::asio::buffer(buffer),
				sender_endpoint,
				0,
				error);

	if (error)
	{
	    const auto interrupted_error =
		boost::system::errc::make_error_code(boost::system::errc::interrupted);

	    if (error.value() == interrupted_error.value())
	    {
		++sequence_number;

		continue;
	    }

	    throw boost::system::system_error {error, "receive_from"};
	}

	++statistics.received_packets;

	if (sender_endpoint != destination_endpoint)
	{
	    /*
	      boost::asio::ip::icmp::socket uses a raw socket,
	      so sometimes it can receive packets from localhost
	    */

	    --statistics.sent_packets;
	    --statistics.received_packets;

	    continue;
	}

	const auto& const_ip_header =
	    *reinterpret_cast<const ping::ip::ipv4_header*>(buffer.data());

	const auto& const_icmp_echo =
	    *reinterpret_cast<const ping::icmp::icmp_echo_header*>(const_ip_header.data());

	std::cout << received_bytes
		  << " received bytes from "
		  << sender_endpoint.address()
		  << ": "
		  << "icmp_sequence_number="
		  << const_icmp_echo.sequence_number()
		  << " ttl="
		  << static_cast<int>(const_ip_header.time_to_live())
		  << std::endl;

	const auto sleep_duration = ::timespec {
	    .tv_sec  = 1,
	    .tv_nsec = 0
	};

	::nanosleep(&sleep_duration, nullptr);

	++sequence_number;
    }
}

void interrupt_signal_handler(int signal)
{
    std::cout << "--- ping statistics ---\n"
	      << statistics
	      << std::endl;

    if (signal == SIGQUIT)
    {
	return;
    }

    std::exit(EXIT_SUCCESS);
}

void alarm_signal_handler(int)
{
    return;
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
	    std::cerr << "Usage: ping -h" << std::endl;

	    return 1;
	}

	ping::configuration::options options;

	options.parse_command_line(argc, argv);

	if (options.contains("-h"))
	{
	    std::cout << options << std::endl;
	}

	else if (not options.contains("-a"))
	{
	    std::cerr << "ping: <address> is requred" << std::endl;

	    return 1;
	}

	else
	{
	    struct ::sigaction interrupt_signal;

	    interrupt_signal.sa_handler = &interrupt_signal_handler;

	    auto quit_signal = interrupt_signal;

	    struct ::sigaction alarm_signal;

	    alarm_signal.sa_handler = &alarm_signal_handler;
	    alarm_signal.sa_flags   = 0;

	    set_signal_handler(SIGINT,  interrupt_signal);
	    set_signal_handler(SIGQUIT, quit_signal);
	    set_signal_handler(SIGALRM, alarm_signal);

	    do_ping(options["-a"].as<std::string>());
	}
    }

    catch (const std::exception& e)
    {
	std::cerr << e.what() << std::endl;

	return 2;
    }

    return 0;
}
