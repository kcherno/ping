#include <string_view>
#include <stdexcept>
#include <utility>
#include <format>
#include <limits>
#include <string>

#include <cstdint>

#include "options/option.hpp"

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

}
