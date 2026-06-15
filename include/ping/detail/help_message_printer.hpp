#pragma once

#include <iostream>
#include <format>

#include "options/grammar.hpp"

#include "ping/generic/executor.hpp"

#include "ping/application.hpp"

namespace ping::detail
{
    class help_message_printer final : public generic::executor
    {
    public:

        constexpr help_message_printer(
            const options::grammar& options) noexcept :
                options_ {options}
        {}

        void execute() override
        {
            std::cout << std::format("usage: {} [<options>] <address>\n",
                application::name());
        }

    private:

        const options::grammar& options_;
    };
}
