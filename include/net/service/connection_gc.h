// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_SERVICE_CONNECTION_GC_H
#define HYPERION_INCLUDE_NET_SERVICE_CONNECTION_GC_H
#pragma once

namespace hyperion::net
{
    class NetData;

    ///
    /// Removes old and no longer active connections
    void ConnectionGCService(NetData& net_data);

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_GC_H
