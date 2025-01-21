#ifndef IPV4_HEADER_HPP__
#define IPV4_HEADER_HPP__

#include <cassert>
#include <cstdint>

extern "C"
{

#include <netinet/ip.h>

}

namespace ping::ip
{
    class ipv4_header final {
    public:
	constexpr uint8_t header_length() const noexcept
	{
	    return ip.ihl * 4;
	}

	std::size_t total_length() const noexcept
	{
	    return ::ntohs(ip.tot_len);
	}

	constexpr uint8_t time_to_live() const noexcept
	{
	    return ip.ttl;
	}

	const uint8_t* data() const noexcept
	{
	    return reinterpret_cast<const uint8_t*>(this) + header_length();
	}

	uint8_t* data() noexcept
	{
	    using const_this = const ipv4_header*;
	    
	    return const_cast<uint8_t*>(const_cast<const_this>(this)->data());
	}

	std::size_t size() const noexcept
	{
	    return total_length() - header_length();
	}

    private:
	::iphdr ip;
    };
}

#endif // IPV4_HEADER_HPP__
