// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_CONNECTION_LISTENER_H
#define HYPERION_INCLUDE_NET_CONNECTION_LISTENER_H
#pragma once

namespace hyperion::net
{
    class NetData;

    ///
    /// Reads data from connections
    void ConnectionListenerService(NetData& net_data);

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_LISTENER_H
