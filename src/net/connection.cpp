// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/connection.h"

#include <asio/read.hpp>
#include <asio/write.hpp>

#include "net/comm_format.h"
#include "net/logger.h"

using namespace hyperion;

net::Connection::Connection(SocketT&& socket)
    : id(nextConnectionId_++), socket(std::move(socket)), timer(this->socket.get_executor()) {
    LOG_MESSAGE_F(
        info, "Created connection %llu, %s", id, this->socket.remote_endpoint().address().to_string().c_str());
}

void net::Connection::End() noexcept {
    try {
        if (socket.is_open()) {
            socket.shutdown(asio::socket_base::shutdown_both);
            socket.close();

            LOG_MESSAGE_F(info, "Connection %llu closed", id);
        }
    }
    catch (std::exception& e_close) {
        LOG_MESSAGE_F(error, "Failed to close socket: %s", e_close.what());
    }
}

void net::Connection::AsyncWrite(
    const ByteVector& msg,
    std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    async_write(
        socket,
        asio::buffer(msg),
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                LOG_MESSAGE_F(debug, "Async write ec: %s", error.message().c_str());
                return;
            }

            callback(error, bytes_transferred);
        });
}

void net::Connection::AsyncRead(
    const std::size_t n, std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback) {

    async_read(
        socket,
        buf.prepare(n),
        [this, callback{std::move(callback)}](const asio::error_code& error, const std::size_t bytes_transferred) {
            if (error) {
                LOG_MESSAGE_F(debug, "Async read ec: %s", error.message().c_str());
            }

            LOG_MESSAGE_F(debug, "Read %llu bytes", bytes_transferred);

            callback(error, bytes_transferred);
        });
}
