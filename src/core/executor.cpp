#include <system_error>
#include <iostream>
#include <string>

#include <csetjmp>
#include <csignal>
#include <cerrno>
#include <ctime>

extern "C"
{

#include <unistd.h>

}

#include <boost/asio.hpp>

#include "ping.hpp"

namespace
{
    struct ::sigaction get_signal_mask(int signal)
    {
	struct ::sigaction action;

	if (::sigaction(signal, nullptr, &action) == -1)
	{
	    const auto error = std::make_error_code(std::errc {errno});

	    throw std::system_error {error, "get_signal_mask"};
	}

	return action;
    }

    void set_signal_handler(int signal, struct ::sigaction& action)
    {
	if (::sigaction(signal, &action, nullptr) == -1)
	{
	    const auto error = std::make_error_code(std::errc {errno});

	    throw std::system_error {error, "set_signal_handler"};
	}
    }

    void sleep_for(const std::chrono::seconds& seconds)
    {
	::timespec sleep_duration {
	    .tv_sec  = seconds.count(),
	    .tv_nsec = 0
	};

	::timespec sleep_reminder;

	while (sleep_duration.tv_sec > 0 || sleep_duration.tv_nsec > 0)
	{
	    if (::nanosleep(&sleep_duration, &sleep_reminder) == -1)
	    {
		if (errno == EINTR)
		{
		    sleep_duration = sleep_reminder;

		    continue;
		}

		else
		{
		    const auto error = std::make_error_code(std::errc {errno});

		    throw std::system_error {error, "sleep_for"};
		}
	    }

	    break;
	}
    }

    ping::statistics::transmission_statistics global_transmission_statistics;

    std::string global_address_destination;
    jmp_buf     global_jump_buffer;

    void terminal_interrupt_handler(int signal)
    {
	std::cout << "\n--- "
		  << global_address_destination
		  << " ping statistics ---\n"
		  << global_transmission_statistics
		  << std::endl;

	if (signal == SIGINT)
	{
	    longjmp(global_jump_buffer, 1);
	}
    }

    void alarm_handler(int)
    {
	return;
    }
}

namespace ping::core
{
    executor::executor(const configuration::options& options) :
	io_context           {},
	socket               {io_context},
	destination_endpoint {},
	sender_endpoint      {},
	error                {},
	buffer               {},
	responce_timeout     {options["-W"].as<int>()},
	repetition_timeout   {options["-i"].as<int>()},
	sequence_number      {1},
	max_sequence_number  {options["-c"].as<int>()}
    {
	const auto address =
	    boost::asio::ip::make_address(options["-a"].as<std::string>(), error);

	if (error)
	{
	    const auto& what = options["-a"].as<std::string>();

	    throw boost::system::system_error {error, what};
	}

	destination_endpoint = boost::asio::ip::icmp::endpoint {address, 0};

	socket.open(destination_endpoint.protocol());

	const auto time_to_live = boost::asio::ip::unicast::hops {
	    options["-t"].as<int>()
	};

	socket.set_option(time_to_live);
    }

    void executor::ping()
    {
	while (sequence_number <= max_sequence_number)
	{
	    auto mutable_buffer = proto::tool::mutable_buffer {
		buffer.data(),
		proto::net::icmp_echo::header_length()
	    };

	    proto::net::make_echo_request(mutable_buffer, 0, sequence_number);

	    socket.send_to(boost::asio::buffer(mutable_buffer.data(),
					       mutable_buffer.size()),
			   destination_endpoint);

	    ++global_transmission_statistics.sent_packets;

	    if (successful_package_receipt())
	    {
		++global_transmission_statistics.received_packets;

		print_received_data();
	    }

	    else
	    {
		if (not sender_endpoint.address().is_unspecified())
		{
		    print_error_message();
		}
	    }

	    if (max_sequence_number - sequence_number)
	    {
		sleep_for(repetition_timeout);
	    }

	    ++sequence_number;
	}
    }

    void executor::print_error_message()
    {
	auto ipv4 =
	    reinterpret_cast<const proto::net::ipv4*>(buffer.data());

	auto icmp_echo =
	    reinterpret_cast<const proto::net::icmp_echo*>(ipv4->data());

	const auto enumerator =
	    proto::net::icmp::type_enumerator {icmp_echo->type()};

	std::cout << "From "
		  << sender_endpoint.address().to_string()
		  << " icmp_sequence_number="
		  << icmp_echo->sequence_number()
		  << ' '
		  << proto::net::icmp::to_string(enumerator)
		  << std::endl;
    }

    void executor::print_received_data()
    {
	auto ipv4 =
	    reinterpret_cast<const proto::net::ipv4*>(buffer.data());

	auto icmp_echo =
	    reinterpret_cast<const proto::net::icmp_echo*>(ipv4->data());

	std::cout << ipv4->total_length()
		  << " bytes from "
		  << sender_endpoint.address().to_string()
		  << ": "
		  << "icmp_sequence_number="
		  << icmp_echo->sequence_number()
		  << " ttl="
		  << ipv4->time_to_live()
		  << std::endl;
    }

    void executor::run()
    {
	auto signal_interrupt_action = get_signal_mask(SIGINT);

	signal_interrupt_action.sa_handler =
	    &terminal_interrupt_handler;

	signal_interrupt_action.sa_flags = 0;

	set_signal_handler(SIGINT,  signal_interrupt_action);

	auto signal_quit_action = get_signal_mask(SIGQUIT);

	signal_quit_action.sa_handler =
	    &terminal_interrupt_handler;

	signal_quit_action.sa_flags = 0;

	set_signal_handler(SIGQUIT, signal_quit_action);

	auto signal_alarm_action = get_signal_mask(SIGALRM);

	signal_alarm_action.sa_handler =
	    &alarm_handler;

	signal_quit_action.sa_flags = 0;

	set_signal_handler(SIGALRM, signal_alarm_action);

	global_address_destination = destination_endpoint.address().to_string();

	switch (setjmp(global_jump_buffer))
	{
	case 0:
	    std::cout << "PING "
		      << global_address_destination
		      << " 8 bytes of data"
		      << std::endl;

	    ping();

	    break;

	default:
	    break;
	}
    }

    bool executor::successful_package_receipt()
    {
	using namespace boost;

	::alarm(responce_timeout.count());

	socket.receive_from(asio::buffer(buffer), sender_endpoint, 0, error);

	if (error)
	{
	    ::alarm(0);
	    
	    const auto interrupt_error =
		system::errc::make_error_code(system::errc::interrupted);

	    if (error.value() == interrupt_error.value())
	    {
		return false;
	    }

	    throw system::system_error {error, "receive_from"};
	}

	::alarm(0);

	if (sender_endpoint != destination_endpoint)
	{
	    return false;
	}

	return true;
    }
}
