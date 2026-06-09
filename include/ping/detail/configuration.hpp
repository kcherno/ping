#pragma once

#include <string_view>
#include <chrono>

#include "options/parser.hpp"

namespace ping::detail
{
    template<typename T>
    inline T get_number_or_default(
        const options::parser::parsed_options& parsed_options,
        std::string_view                       option,
        T                                      default_value)
    {
        if (parsed_options.contains(option))
        {
            return static_cast<T>(
                std::stoi(
                    std::string(
                        parsed_options[option].front()
                    )
                )
            );
        }

        return default_value;
    }

    inline std::chrono::seconds get_duration_or_default(
        const options::parser::parsed_options& parsed_options,
        std::string_view                       option,
        std::chrono::seconds                   default_value)
    {
        return std::chrono::seconds {
            get_number_or_default<int>(
                parsed_options,
                option,
                default_value.count()
            )
        };
    }
}
