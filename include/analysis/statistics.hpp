#ifndef STATISTICS_HPP__
#define STATISTICS_HPP__

#include <iomanip>
#include <ostream>

namespace ping::analysis
{
    struct statistics final {
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

	std::size_t sent_packets;
	std::size_t received_packets;
    };

    template<typename T>
    std::basic_ostream<T>& operator<<(std::basic_ostream<T>& ostream,
				      const statistics&      statistics)
    {
	return ostream << statistics.sent_packets
		       << " sent, "
		       << statistics.received_packets
		       << " received, "
		       << std::setprecision(3)
		       << statistics.packet_loss_percentage()
		       << "% packet loss";
    }
}

#endif // STATISTICS_HPP__
