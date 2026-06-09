#pragma once

#include <string_view>
#include <utility>
#include <memory>

#include <cstdint>

#include "options/grammar.hpp"
#include "options/parser.hpp"

#include "ping/configuration.hpp"

namespace ping
{
    class application final
    {
    public:

        class basic_application_executor
        {
        public:

            virtual ~basic_application_executor() = default;

            virtual void execute() = 0;
        };

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

        void initialize_executor();

        configuration                               configuration_;
        std::unique_ptr<basic_application_executor> executor_;
        options::grammar                            options_;
        statistics                                  statistics_;
    };
}
