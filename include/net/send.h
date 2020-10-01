// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_SEND_H
#define HYPERION_INCLUDE_NET_SEND_H
#pragma once

#include <asio/ip/tcp.hpp>
#include <string>

namespace hyperion::net
{
    class NetData;

    // Sends data through connections

    void Send(asio::ip::tcp::socket& socket, const std::string& message);

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_SEND_H
