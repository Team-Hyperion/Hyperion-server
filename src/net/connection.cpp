// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/connection.h"

#include <asio/read_until.hpp>
#include <asio/write.hpp>

#include "net/comm_format.h"
#include "net/net_logger.h"

using namespace hyperion;

void net::Connection::End() noexcept {
    try {
        socket.shutdown(asio::socket_base::shutdown_both);
        socket.close();
    }
    catch (std::exception& e_close) {
        LOG_MESSAGE_F(error, "Failed to close socket: %s", e_close.what());
        // TODO how do we clean this up?
    }
}

void net::Connection::AsyncWrite(
    const ByteVector& msg,
    std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    assert(msg[msg.size() - 1] == CommFormat::kMessageTerminator); // Messages must be terminated

    async_write(
        socket,
        asio::buffer(msg),
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                NET_LOG_F(error, "Async write ec: %s", error.message().c_str());
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
                NET_LOG_F(error, "Async read ec: %s", error.message().c_str());
            }

            const auto bytes_transferred_unterminated = bytes_transferred - sizeof CommFormat::kMessageTerminator;

            LOG_MESSAGE_F(debug,
                          "Read %lld bytes (with terminator), %lld bytes (without terminator)",
                          bytes_transferred,
                          bytes_transferred_unterminated);

            LOG_MESSAGE_F(debug, "Received %s", asio::buffer_cast<const char*>(buf.data()));

            callback(error, bytes_transferred_unterminated);
        });
}
