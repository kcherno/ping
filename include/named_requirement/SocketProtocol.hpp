#pragma once

#include <concepts>

namespace ping::named_requirement
{
    template<typename T>
    concept SocketProtocol = requires
    {
	{ T::socket_protocol() } noexcept -> std::same_as<int>;
    };
}
