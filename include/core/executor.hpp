#ifndef CORE_EXECUTOR_HPP__
#define CORE_EXECUTOR_HPP__

#include <chrono>
#include <array>

#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include "ping.hpp"

namespace ping::core
{
    class executor final {
    public:
	executor(const configuration::options&);

	void run();

    private:
	void ping();

	void print_error_message();

	void print_received_data();

	bool successful_package_receipt();
	
	boost::asio::io_context io_context;

	boost::asio::ip::icmp::socket   socket;
	boost::asio::ip::icmp::endpoint destination_endpoint;
	boost::asio::ip::icmp::endpoint sender_endpoint;

	boost::system::error_code error;

	std::array<char, 32> buffer;

	std::chrono::seconds responce_timeout;
	std::chrono::seconds repetition_timeout;

	int sequence_number;
	int max_sequence_number;
    };
}

#endif // CORE_EXECUTOR_HPP__
