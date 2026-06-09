#pragma once

#include <string_view>
#include <utility>
#include <chrono>
#include <limits>

#include <cstdint>

#include "options/parser.hpp"

#include "detail/configuration.hpp"

namespace ping
{
    class configuration final
    {
    public:

        using options_type = std::pair<
            options::parser::parsed_options,
            options::parser::positional_options
        >;

        static constexpr std::string_view default_address {};

        static constexpr int default_count = std::numeric_limits<
            std::uint16_t>::max();

        static constexpr auto default_identifier = std::numeric_limits<
            std::uint16_t>::min();

        static constexpr std::chrono::seconds default_interval {1};

        static constexpr std::string_view default_message {};

        static constexpr std::chrono::seconds default_timeout {1};

        configuration(const options_type& options) :
            address_ {
                options.second.empty() ?
                    default_address :
                    options.second.front()
            },

            count_ {
                detail::get_number_or_default(
                    options.first,
                    "-c",
                    default_count
                )
            },

            identifier_ {
                detail::get_number_or_default(
                    options.first,
                    "--identifier",
                    default_identifier
                )
            },

            interval_ {
                detail::get_duration_or_default(
                    options.first,
                    "-i",
                    default_interval
                )
            },

            message_ {
                options.first.contains("-m") ?
                    options.first["-m"].front() :
                    default_message
            },

            timeout_ {
                detail::get_duration_or_default(
                    options.first,
                    "-t",
                    default_timeout
                )
            }
        {}

        constexpr std::string_view address() const noexcept
        {
            return address_;
        }

        constexpr int count() const noexcept
        {
            return count_;
        }

        constexpr std::uint16_t identifier() const noexcept
        {
            return identifier_;
        }

        constexpr std::chrono::seconds interval() const noexcept
        {
            return interval_;
        }

        constexpr std::string_view message() const noexcept
        {
            return message_;
        }

        constexpr std::chrono::seconds timeout() const noexcept
        {
            return timeout_;
        }

    private:

        std::string_view     address_;
        int                  count_;
        std::uint16_t        identifier_;
        std::chrono::seconds interval_;
        std::string_view     message_;
        std::chrono::seconds timeout_;
    };
}
