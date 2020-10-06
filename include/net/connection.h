// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_CONNECTION_H
#define HYPERION_INCLUDE_NET_CONNECTION_H
#pragma once

#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>
#include <asio/streambuf.hpp>
#include <functional>

#include "hyperion.h"

namespace hyperion::net
{
    ///
    /// A connection with a client
    class Connection
    {
    public:
        /// Bytes
        static constexpr auto kReceiveBufSize = 1000;

        using SocketT = asio::ip::tcp::socket;

        enum class Status
        {
            awaiting_c_greeting,
            active // Client greeting received and accepted
        };


        explicit Connection(SocketT&& socket) : socket(std::move(socket)), timer(this->socket.get_executor()) {}

        Connection(const Connection& other)     = delete;
        Connection(Connection&& other) noexcept = delete;


        ///
        /// Wrapper around asio::async_read_until
        /// Forwards results to callback
        void AsyncReadUntil(
            std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback);


        [[nodiscard]] Status GetStatus() const {
            return status_;
        }
        void SetStatus(const Status status) {
            this->status_ = status;
        }

        SocketT socket;
        asio::streambuf buf{kReceiveBufSize};

        /// Use for timeouts
        asio::steady_timer timer;

    private:
        Status status_ = Status::awaiting_c_greeting;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_H
