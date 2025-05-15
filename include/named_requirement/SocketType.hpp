#pragma once

#include <concepts>

namespace ping::name_requirement
{
    template<typename T>
    concept SocketType = requires
    {
	    { T::socket_type() } noexcept -> std::same_as<int>;
    };
}
