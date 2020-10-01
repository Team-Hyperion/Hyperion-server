// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/send.h"

#include "hyperion.h"

#include "net/comm_format.h"

using namespace hyperion;

void net::Send(asio::ip::tcp::socket& socket, const std::string& message) {
    assert(*message.end() == CommFormat::kMessageTerminator); // Messages must be terminated

    socket.send(asio::buffer(message));
}
