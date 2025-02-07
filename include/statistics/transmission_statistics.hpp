#ifndef STATISTICS_TRANSMISSION_STATISTICS_HPP__
#define STATISTICS_TRANSMISSION_STATISTICS_HPP__

#include <iomanip>
#include <ostream>

namespace ping::statistics
{
    struct transmission_statistics final {
	constexpr double packet_loss_percentage() const noexcept
	{
	    if (sent_packets == received_packets)
	    {
		return 0;
	    }

	    if (sent_packets > 0 && received_packets == 0)
	    {
		return 100;
	    }

	    double lost_packets = sent_packets - received_packets;

	    return (lost_packets / sent_packets) * 100;
	}

	constexpr std::size_t errors() const noexcept
	{
	    return sent_packets - received_packets;
	}

	std::size_t sent_packets;
	std::size_t received_packets;
    };

    template<typename CharT>
    std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>&     ostream,
					  const transmission_statistics& statistics)
    {
	ostream << statistics.sent_packets
		<< " packets transmitted, "
		<< statistics.received_packets
		<< " received, ";

	if (statistics.errors())
	{
	    ostream << '+'
		    << statistics.errors()
		    << ' '
		    << (statistics.errors() == 1 ? "error, " : "errors, ");
	}

	ostream << std::setprecision(3)
		<< statistics.packet_loss_percentage()
		<< "% packet loss";

	return ostream;
    }
}

#endif // STATISTICS_TRANSMISSION_STATISTICS_HPP__
