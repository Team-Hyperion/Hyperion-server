// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_NET_PROP_H
#define HYPERION_INCLUDE_NET_NET_PROP_H
#pragma once

#include <chrono>

namespace hyperion::net
{
    ///
    /// Configures server's networking properties
    struct NetProp
    {
        using PortT = unsigned short;

        enum class InternetProtocol
        {
            v4,
            v6,
        };

        InternetProtocol netProtocol = InternetProtocol::v4;
        PortT portNum                = 34200;

        // Timeouts
        std::chrono::milliseconds toSendServerGreeting{5000};
        std::chrono::milliseconds toReceiveClientGreeting{5000};

        // Amount of time to wait before retrying

        std::chrono::milliseconds sleepAcceptorInitFailed{1000};
        std::chrono::milliseconds sleepGcPass{20000};
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_NET_PROP_H
