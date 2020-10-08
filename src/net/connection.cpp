// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/connection.h"

#include <asio/read_until.hpp>
#include <asio/write.hpp>

#include "net/comm_format.h"
#include "net/logger.h"

using namespace hyperion;

void net::Connection::End() noexcept {
    try {
        if (socket.is_open()) {
            socket.shutdown(asio::socket_base::shutdown_both);
            socket.close();
        }
    }
    catch (std::exception& e_close) {
        LOG_MESSAGE_F(error, "Failed to close socket: %s", e_close.what());
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

    async_read_until(
        socket,
        buf,
        CommFormat::kMessageTerminator,
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                NET_LOG_F(error, "Async read ec: %s", error.message().c_str());
            }

            LOG_MESSAGE_F(debug, "Read %llu bytes (with terminator)", bytes_transferred);

            LOG_MESSAGE_F(debug, "Received %s", asio::buffer_cast<const char*>(buf.data()));

            callback(error, bytes_transferred);
        });
}

std::size_t net::Connection::GetBytesUnterminated(const std::size_t terminated_bytes) noexcept {
    auto bytes_transferred_unterminated = static_cast<decltype(terminated_bytes)>(0);
    if (terminated_bytes > sizeof CommFormat::kMessageTerminator) {
        bytes_transferred_unterminated = terminated_bytes - sizeof CommFormat::kMessageTerminator;
    }

    return bytes_transferred_unterminated;
}
