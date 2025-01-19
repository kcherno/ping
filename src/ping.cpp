#include <string_view>
#include <exception>
#include <stdexcept>
#include <iostream>

#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#include "ping.hpp"

template<typename ExecutionContext>
void do_ping(ExecutionContext& executor, std::string_view address)
{
    auto error = boost::system::error_code {};

    auto destination_address = boost::asio::ip::make_address(address, error);

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

    auto icmp_echo_header = ping::icmp::icmp_echo_header {1, 1};

    socket.send_to(boost::asio::const_buffer(&icmp_echo_header,
					     icmp_echo_header.header_length()),
		   endpoint);

    std::array<uint8_t, 32> unused_buffer;

    socket.receive_from(boost::asio::buffer(unused_buffer), endpoint);
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

	auto io_context = boost::asio::io_context {};

	do_ping(io_context, argv[1]);

	io_context.run();
    }

    catch (const std::exception& e)
    {
	std::cerr << e.what() << std::endl;

	return 1;
    }

    return 0;
}
