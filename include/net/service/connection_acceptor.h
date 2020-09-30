// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
#define HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
#pragma once

namespace hyperion::net
{
    class NetData;

    ///
    /// Accepts connections
    void ConnectionAcceptorService(NetData& net_data);

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
