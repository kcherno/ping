#pragma once

#include <string_view>

namespace ping
{
    class application final
    {
    public:

        application(int argc, char** argv)
        {

        }

        static consteval std::string_view name() noexcept
        {
            return "ping";
        }

        void run() const
        {

        }
    };
}
