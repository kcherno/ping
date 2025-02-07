#include <limits>
#include <string>

#include <cstdint>

#include <boost/program_options.hpp>

#include "configuration/options.hpp"

namespace
{
    void c_notifier(const int& c)
    {
	if (c < 1 || std::numeric_limits<std::uint16_t>::max() < c)
	{
	    constexpr auto what =
		"the valid range for the '-c' option is from 1 to 65535";

	    throw boost::program_options::error {what};
	}
    }

    void i_notifier(const int& i)
    {
	if (i < 0)
	{
	    constexpr auto what =
		"the argument for '-i' option cannot be negative";

	    throw boost::program_options::error {what};
	}
    }

    void t_notifier(const int& t)
    {
	if (t < 1 || std::numeric_limits<std::uint8_t>::max() < t)
	{
	    constexpr auto what =
		"the valid range for the '-t' option is from 1 to 255";

	    throw boost::program_options::error {what};
	}
    }

    void W_notifier(const int& W)
    {
	if (W < 1)
	{
	    constexpr auto what =
		"the argument for '-W' option cannot be negative or zero";

	    throw boost::program_options::error {what};
	}
    }
}

namespace ping::configuration
{
    options::options()
    {
	description.add_options()
	    (",a",
	     boost::program_options::value<std::string>()
	     ->value_name("<address>"),
	     "ip address")

	    (",c",
	     boost::program_options::value<int>()
	     ->default_value(std::numeric_limits<std::uint16_t>::max(), "")
	     ->notifier(c_notifier)
	     ->value_name("<count>"),
	     "stop after <count> replies")

	    (",h",
	     "print help and exit")

	    (",i",
	     boost::program_options::value<int>()
	     ->default_value(1, "")
	     ->notifier(i_notifier)
	     ->value_name("<interval>"),
	     "seconds between sending each packet")

	    (",t",
	     boost::program_options::value<int>()
	     ->default_value(std::numeric_limits<std::uint8_t>::max(), "")
	     ->notifier(t_notifier)
	     ->value_name("<ttl>"),
	     "time to live")

	    (",W",
	     boost::program_options::value<int>()
	     ->default_value(1, "")
	     ->notifier(W_notifier)
	     ->value_name("<timeout>"),
	     "time to wait for response");

	positional_options.add("-a", 1);
    }

    void options::parse_command_line(int argc, char** argv)
    {
	using namespace boost::program_options;

	store(boost::program_options::command_line_parser(argc, argv)
	      .options(description)
	      .positional(positional_options)
	      .style(command_line_style::allow_long           |
		     command_line_style::allow_short          |
		     command_line_style::allow_dash_for_short |
		     command_line_style::long_allow_adjacent  |
		     command_line_style::short_allow_next)
	      .run(),
	      map);

	map.notify();
    }
}
