#ifndef ICMP_HEADER_INTERFACE_HPP__
#define ICMP_HEADER_INTERFACE_HPP__

#include <cstdint>

namespace ping::icmp
{
    template<typename T>
    class icmp_header_interface {
    public:
	constexpr uint8_t type() const noexcept
	{
	    return type_;
	}

	constexpr void type(uint8_t other) noexcept
	{
	    type_ = other;
	}
	
	constexpr uint8_t code() const noexcept
	{
	    return code_;
	}

	constexpr void code(uint8_t other) noexcept
	{
	    code_ = other;
	}

	void calculate_checksum(std::size_t size) noexcept
	{
	    checksum_ = 0;

	    auto sum = uint32_t {};

	    auto data = reinterpret_cast<const uint16_t*>(this);

	    for (; size; size -= 2)
	    {
		sum += *data++;
	    }

	    if (size == 1)
	    {
		sum += *reinterpret_cast<const uint8_t*>(data);
	    }

	    while (sum >> 16)
	    {
		sum += (sum >> 16) + (sum & 0xFFFF);
	    }

	    checksum_ = static_cast<uint16_t>(~sum);
	}

	const uint8_t* data() const noexcept
	{
	    return reinterpret_cast<const uint8_t*>(this) + header_length();
	}

	uint8_t* data() noexcept
	{
	    using const_this = const icmp_header_interface*;

	    return const_cast<uint8_t*>(const_cast<const_this>(this)->data());
	}

	constexpr std::size_t header_length() const noexcept
	{
	    return static_cast<const T*>(this)->header_length();
	}

    protected:
	constexpr icmp_header_interface(uint8_t type, uint8_t code) noexcept :
	    type_     {type},
	    code_     {code},
	    checksum_ {0}
	{}

    private:
	uint8_t  type_;
	uint8_t  code_;
	uint16_t checksum_;
    };
}

#endif // ICMP_HEADER_INTERFACE_HPP__
