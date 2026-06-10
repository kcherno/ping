#pragma once

#include "ping/generic/executor.hpp"

#include "ping/configuration.hpp"

namespace ping::detail
{
    class icmp_executor final : public generic::executor
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
