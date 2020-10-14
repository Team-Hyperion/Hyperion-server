// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_SERVICE_CONNECTION_LISTENER_H
#define HYPERION_INCLUDE_NET_SERVICE_CONNECTION_LISTENER_H
#pragma once

namespace hyperion::net
{
    class Connection;

    // Listens to a connection and dispatches received data

    ///
    /// Begins receiving from connection's socket until socket is terminated
    void BeginAsyncReceive(Connection& connection);

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_SERVICE_CONNECTION_LISTENER_H
