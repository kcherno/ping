#pragma once

#include <iostream>

#include "options/grammar.hpp"

#include "ping/generic/executor.hpp"

namespace ping::detail
{
    class application_options_printer final : public generic::executor
    {
    public:

        constexpr application_options_printer(
            const options::grammar& options) noexcept :
                options_ {options}
        {}

        void execute() override
        {
            std::cout << "usage: ping [<options>] <address>\n\n"
                      << options_
                      << std::endl;
        }

    private:

        const options::grammar& options_;
    };
}
