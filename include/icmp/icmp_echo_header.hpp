#ifndef ICMP_ECHO_HEADER_HPP__
#define ICMP_ECHO_HEADER_HPP__

#include <cstdint>

#include "icmp_header_interface.hpp"

namespace ping::icmp
{
    class icmp_echo_header final : public icmp_header_interface<icmp_echo_header> {
    public:
	icmp_echo_header(uint16_t identifier      = 0,
			 uint16_t sequence_number = 0)
	    noexcept
	    :
	    icmp_header_interface {8, 0},
	    identifier_           {::htons(identifier)},
	    sequence_number_      {::htons(sequence_number)}
	{
	    calculate_checksum(header_length());
	}

	uint16_t identifier() const noexcept
	{
	    return ::ntohs(identifier_);
	}

	void identifier(uint16_t other) noexcept
	{
	    identifier_ = ::htons(other);
	}

	uint16_t sequence_number() const noexcept
	{
	    return ::ntohs(sequence_number_);
	}

	void sequence_number(uint16_t other) noexcept
	{
	    sequence_number_ = ::htons(other);
	}

	constexpr std::size_t header_length() const noexcept
	{
	    return 8;
	}

    private:
	uint16_t identifier_;
	uint16_t sequence_number_;
    };
}

#endif // ICMP_ECHO_HEADER_HPP__
