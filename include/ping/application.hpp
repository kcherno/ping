#pragma once

#include <string_view>
#include <utility>
#include <memory>

#include "options/grammar.hpp"
#include "options/parser.hpp"

#include "ping/generic/executor.hpp"

#include "ping/configuration.hpp"
#include "ping/statistics.hpp"

namespace ping
{
    class application final
    {
    public:

        application(int, char**);

        static consteval std::string_view name() noexcept
        {
            return "ping";
        }

        void run() const
        {
            executor_->execute();
        }

    public:

        void initialize_executor(
            const std::pair<
                options::parser::parsed_options,
                options::parser::positional_options>&
        );

        configuration                      configuration_;
        std::unique_ptr<generic::executor> executor_;
        options::grammar                   options_;
        statistics                         statistics_;
    };
}
