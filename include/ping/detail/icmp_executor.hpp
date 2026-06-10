#pragma once

#include "ping/configuration.hpp"
#include "ping/application.hpp"

namespace ping::detail
{
    class icmp_executor final : public application::basic_application_executor
    {
    public:

        constexpr icmp_executor(
            const configuration&      configuration,
            application::statistics& statistics
        ) noexcept :
            configuration_ {configuration},
            statistics_    {statistics}
        {}

        void execute() override;

    private:

        const configuration&     configuration_;
        application::statistics& statistics_;
    };
}
