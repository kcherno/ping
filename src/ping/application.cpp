#include <string_view>
#include <stdexcept>
#include <utility>
#include <format>
#include <limits>
#include <memory>
#include <string>

#include <cstdint>

#include "options/option.hpp"

#include "ping/detail/icmp_ping_executor.hpp"

#include "ping/application.hpp"

ping::application::application(int argc, char** argv) :
    options_ {
        options::option {
            .short_name  = "-4",
            .long_name   = "--ipv4",
            .description = "use IPv4 address"
        },

        options::option {
            .short_name        = "-c",
            .long_name         = "--count",
            .description       = "number of packets to send",
            .has_arguments     = true,
            .argument_verifier = [](std::string_view argument)
            {
                try
                {
                    if (not (std::stoi(std::string(argument)) > 0))
                    {
                        throw std::logic_error {
                            std::format(
                                "argument_verifier: "
                                "number of packets must be > 0"
                            )
                        };
                    }
                }

                catch (const std::invalid_argument& exception)
                {
                    throw std::logic_error {
                        std::format(
                            "argument_verifier: "
                            "{} is an invalid argument for [-c, --count]",
                            argument
                        )
                    };
                }
            }
        },

        options::option {
            .short_name        = "-i",
            .long_name         = "--interval",
            .description       = "interval between pings in seconds",
            .has_arguments     = true,
            .argument_verifier = [](std::string_view argument)
            {
                try
                {
                    if (not (std::stoi(std::string(argument)) > 0))
                    {
                        throw std::logic_error {
                            std::format(
                                "argument_verifier: interval must be > 0"
                            )
                        };
                    }
                }

                catch (const std::invalid_argument& exception)
                {
                    throw std::runtime_error {
                        std::format(
                            "argument_verifier: "
                            "{} is an invalid argument for [-i, --interval]",
                            argument
                        )
                    };
                }
            }
        },

        options::option {
            .long_name         = "--identifier",
            .description       = "ICMP echo identifier",
            .has_arguments     = true,
            .argument_verifier = [](std::string_view argument)
            {
                try
                {
                    const auto identifier = std::stoi(std::string(argument));

                    if (not std::in_range<std::uint16_t>(identifier))
                    {
                        throw std::logic_error {
                            std::format(
                                "argument_verifier: "
                                "identifier must be in the range "
                                "from {} to {}",
                                std::numeric_limits<std::uint16_t>::min(),
                                std::numeric_limits<std::uint16_t>::max()
                            )
                        };
                    }
                }

                catch (const std::invalid_argument& exception)
                {
                    throw std::logic_error {
                        std::format(
                            "argument_verifier: "
                            "{} is invalid argument for [--identifier]",
                            argument
                        )
                    };
                }
            }
        },

        options::option {
            .short_name    = "-m",
            .long_name     = "--message",
            .description   = "ICMP echo message",
            .has_arguments = true
        },

        options::option {
            .short_name        = "-t",
            .long_name         = "--timeout",
            .description       = "response timeout in seconds",
            .has_arguments     = true,
            .argument_verifier = [](std::string_view argument)
            {
                try
                {
                    if (not (std::stoi(std::string(argument)) > 0))
                    {
                        throw std::logic_error {
                            std::format(
                                "argument_verifier: timeout must be > 0"
                            )
                        };
                    }
                }

                catch (const std::invalid_argument& exception)
                {
                    throw std::logic_error {
                        std::format(
                            "argument_verifier: "
                            "{} is an invalid argument for [-t, --timeout]",
                            argument
                        )
                    };
                }
            }
        }
    },

    statistics_ {
        .received_packets = 0,
        .sent_packets     = 0
    }
{
    auto options = options::parser(options_).parse_command_line(argc, argv);

    initialize_configuration(options);

    initialize_executor();
}

void ping::application::initialize_configuration(
    const options_type& options) noexcept
{
    const auto& [parsed_options, positional_options] = options;

    if (parsed_options.contains("-c"))
    {
        configuration_.count = static_cast<decltype(configuration_.count)>(
            std::stoi(std::string(parsed_options["-c"].front())));
    }

    else
    {
        configuration_.count =
            std::numeric_limits<decltype(configuration_.count)>::max();
    }

    if (positional_options.empty())
    {
        configuration_.destination = {};
    }

    else
    {
        configuration_.destination = positional_options.front();
    }

    if (parsed_options.contains("--identifier"))
    {
        configuration_.identifier = static_cast<decltype(
            configuration_.identifier)>(std::stoi(std::string(
                parsed_options["--identifier"].front())));
    }

    else
    {
        configuration_.identifier = std::numeric_limits<decltype(
            configuration_.identifier)>::min();
    }

    if (parsed_options.contains("-i"))
    {
        configuration_.interval = std::chrono::seconds(std::stoi(std::string(
            parsed_options["-i"].front())));
    }

    else
    {
        configuration_.interval = std::chrono::seconds {1};
    }

    if (parsed_options.contains("-m"))
    {
        configuration_.message = parsed_options["-m"].front();
    }

    else
    {
        configuration_.message = {};
    }

    if (parsed_options.contains("-t"))
    {
        configuration_.timeout = std::chrono::seconds(std::stoi(std::string(
            parsed_options["-t"].front())));
    }

    else
    {
        configuration_.timeout = std::chrono::seconds {1};
    }
}

void ping::application::initialize_executor()
{
    if (configuration_.destination.empty())
    {

    }

    else
    {
        executor_ = std::make_unique<detail::icmp_ping_executor>(
            configuration_, statistics_);
    }
}
