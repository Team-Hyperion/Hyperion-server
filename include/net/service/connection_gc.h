// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_SERVICE_CONNECTION_GC_H
#define HYPERION_INCLUDE_NET_SERVICE_CONNECTION_GC_H
#pragma once

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

namespace hyperion::net
{
    class NetData;

    ///
    /// Sweeps inactive connections
    ///
    /// Befriends Connections access connections_
    class ConnectionGc
    {
    public:
        ///
        /// \param context Sweep timer attached to context, ensure lifetime outlasts this object
        /// \param net_data Defines sweep times and connection to sweep, ensure lifetime outlasts this object
        explicit ConnectionGc(asio::io_context& context, NetData& net_data) : netData_(net_data), timer_(context) {}

        ///
        /// Begins intermittent gc sweeps, stops when provided io_context stops
        void Start();

    private:
        ///
        /// Sets up async_wait for running Sweep after a set time
        void SetupSweepTimer();

        ///
        /// Removes old and no longer active connections
        static void Sweep(NetData& net_data);

        NetData& netData_;
        asio::steady_timer timer_;
    };


} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_GC_H
