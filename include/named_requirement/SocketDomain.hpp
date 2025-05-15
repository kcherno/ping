#pragma once

#include <concepts>

namespace ping::named_requirement
{
    template<typename T>
    concept SocketDomain = requires
    {
	{ T::socket_domain() } noexcept -> std::same_as<int>;
    };
}
