#pragma once

extern "C"
{

#include <netinet/in.h>
#include <sys/socket.h>

}

#include "generic/datagram_socket.hpp"
#include "generic/basic_socket.hpp"

namespace ping::core
{
    class ip final
    {
    public:

	consteval static int socket_domain() noexcept
	{
	    return AF_INET;
	}

	class icmp final
	{
	public:

	    consteval static int socket_protocol() noexcept
	    {
		return IPPROTO_ICMP;
	    }

	    using socket =
		generic::basic_socket<ip, generic::datagram_socket, icmp>;
	};
    };
}
