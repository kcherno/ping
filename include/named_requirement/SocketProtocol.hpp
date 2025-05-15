#pragma once

#include <concepts>

namespace ping::name_requirement
{
    template<typename T>
    concept SocketProtocol = requires
    {
	{ T::socket_protocol() } noexcept -> std::same_as<int>;
    };
}
