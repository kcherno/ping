#pragma once

#include <source_location>
#include <system_error>
#include <optional>
#include <utility>

extern "C"
{

#include <sys/socket.h>
#include <unistd.h>

}

#include "named_requirement/SocketProtocol.hpp"
#include "named_requirement/SocketDomain.hpp"
#include "named_requirement/SocketType.hpp"

namespace ping::generic
{
    template<named_requirement::SocketDomain   Domain,
	     named_requirement::SocketType     Type,
	     named_requirement::SocketProtocol Protocol>
    class basic_socket final
    {
    public:

	basic_socket() = default;

	basic_socket(const basic_socket&) = delete;

	basic_socket(basic_socket&& other) noexcept :
	    native_handler_ {std::move(other.native_handler_)}
	{}

	basic_socket& operator=(const basic_socket&) = delete;

	basic_socket& operator=(basic_socket&& other) noexcept
	{
	    if (this != &other)
	    {
		native_handler_.swap(other.native_handler_);
	    }

	    return *this;
	}

	~basic_socket()
	{
	    close();
	}

	void open()
	{
	    std::error_code error;

	    if (open(error); error)
	    {
		throw std::system_error {
		    error,
		    std::source_location::current().function_name()
	        };
	    }
	}

	void open(std::error_code& error) noexcept
	{
	    constexpr auto domain   = Domain::socket_domain();
	    constexpr auto type     = Type::socket_type();
	    constexpr auto protocol = Protocol::socket_protocol();

	    if (auto fd = ::socket(domain, type, protocol); fd != -1)
	    {
		native_handler_ = fd;
	    }

	    else
	    {
		error = std::make_error_code(std::errc {errno});
	    }
	}

	void close() noexcept
	{
	    if (native_handler_.has_value())
	    {
		::close(native_handler_.value());
	    }

	    native_handler_.reset();
	}

    private:

	std::optional<int> native_handler_;
    };
}
