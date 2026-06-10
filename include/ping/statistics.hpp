#pragma once

namespace ping
{
    struct statistics final
    {
        std::size_t received_packets;
        std::size_t sent_packets;
    };
}
