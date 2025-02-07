#include <exception>
#include <iostream>

#include "ping.hpp"

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
	    ping::core::executor(options).run();
	}
    }

    catch (const std::exception& e)
    {
	std::cerr << e.what() << std::endl;

	return 2;
    }

    return 0;
}
