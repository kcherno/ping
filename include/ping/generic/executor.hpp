#pragma once

namespace ping::generic
{
    class executor
    {
    public:

        virtual ~executor() = default;

        virtual void execute() = 0;
    };
}
