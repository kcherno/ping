#pragma once

#include <string_view>
#include <memory>

#include <cstdint>

#include "options/grammar.hpp"

#include "ping/generic/executor.hpp"

#include "ping/configuration.hpp"

namespace ping
{
    class application final
    {
    public:

        struct statistics final
        {
            std::uint16_t received_packets;
            std::uint16_t sent_packets;
        };

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

        configuration                      configuration_;
        std::unique_ptr<generic::executor> executor_;
        options::grammar                   options_;
        statistics                         statistics_;
    };
}
