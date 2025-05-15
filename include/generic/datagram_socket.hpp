#pragma once

extern "C"
{

#include <sys/socket.h>

}

namespace ping::generic
{
    class datagram_socket final
    {
    public:

	consteval static int socket_type() noexcept
	{
	    return SOCK_DGRAM;
	}
    };
}
