#pragma once

#include <string_view>
#include <utility>
#include <chrono>
#include <memory>

#include <cstdint>

#include "options/grammar.hpp"
#include "options/parser.hpp"

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

        struct configuration final
        {
            std::uint16_t        count;
            std::string_view     destination;
            std::uint16_t        identifier;
            std::chrono::seconds interval;
            std::string_view     message;
            std::chrono::seconds timeout;
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

        using options_type = std::pair<
            options::parser::parsed_options,
            options::parser::positional_options>;

        void initialize_configuration(const options_type&) noexcept;

        void initialize_executor();

        configuration                               configuration_;
        std::unique_ptr<basic_application_executor> executor_;
        options::grammar                            options_;
        statistics                                  statistics_;
    };
}
