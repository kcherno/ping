#include <limits>
#include <string>

#include <cstdint>

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/parsers.hpp>

#include "configuration/options.hpp"

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
	     boost::program_options::value<std::size_t>()
	     ->default_value(std::numeric_limits<uint16_t>::max(), "")
	     ->value_name("<count>"),
	     "stop after <count> replies")

	    (",h",
	     "print help and exit")

	    (",i",
	     boost::program_options::value<std::size_t>()
	     ->default_value(0, "")
	     ->value_name("<interval>"),
	     "seconds between sending each packet")

	    (",t",
	     boost::program_options::value<uint8_t>()
	     ->default_value(std::numeric_limits<uint8_t>::max(), "")
	     ->value_name("<ttl>"),
	     "time to live")

	    (",W",
	     boost::program_options::value<std::size_t>()
	     ->default_value(0, "")
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
