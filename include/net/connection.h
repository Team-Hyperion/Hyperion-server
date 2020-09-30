// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_CONNECTION_H
#define HYPERION_INCLUDE_NET_CONNECTION_H
#pragma once

#include <asio/ip/tcp.hpp>

namespace hyperion::net
{
    ///
    /// A connection with a client
    class Connection
    {
    public:
        using SocketT = asio::ip::tcp::socket;


        explicit Connection(SocketT&& socket) : socket(std::move(socket)) {}

        SocketT socket;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_H
