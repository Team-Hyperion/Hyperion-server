// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/connection.h"

#include <asio/read_until.hpp>
#include <asio/write.hpp>

#include "net/comm_format.h"

using namespace hyperion;

void net::Connection::AsyncWrite(
    const ByteVector& msg,
    std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    assert(msg[msg.size() - 1] == CommFormat::kMessageTerminator); // Messages must be terminated

    async_write(
        socket,
        asio::buffer(msg),
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                LOG_MESSAGE_F(error, "Failed performing async write: %s", error.message().c_str());
                return;
            }

            callback(error, bytes_transferred);
        });
}

void net::Connection::AsyncReadUntil(
    std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    // TODO replace a with static delimiter
    async_read_until(
        socket,
        buf,
        'a',
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                LOG_MESSAGE_F(error, "Error performing async read: %s", error.message().c_str());
            }

            LOG_MESSAGE_F(debug, "Read %lld bytes", bytes_transferred);
            LOG_MESSAGE_F(debug, "Received %s", asio::buffer_cast<const char*>(buf.data()));

            callback(error, bytes_transferred);
        });
}
