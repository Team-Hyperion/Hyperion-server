// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_CONNECTION_H
#define HYPERION_INCLUDE_NET_CONNECTION_H
#pragma once

#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>
#include <asio/streambuf.hpp>
#include <functional>

#include "hyperion.h"
#include "media/media_prop.h"
#include "net/type_alias.h"

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
            send_s_greeting,
            awaiting_c_greeting,
            active // Client greeting received and accepted
        };


        explicit Connection(SocketT&& socket) : socket(std::move(socket)), timer(this->socket.get_executor()) {}

        Connection(const Connection& other)     = delete;
        Connection(Connection&& other) noexcept = delete;


        ///
        /// If socket is open, calls terminate and close on socket
        /// Connection may not be used after this, a new one must be constructed
        void End() noexcept;

        ///
        /// Wrapper around asio::async_write
        /// Forwards results to callback
        /// \param msg Ensure lifetime lasts until callback is called
        void AsyncWrite(const ByteVector& msg,
                        std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback);

        ///
        /// Wrapper around asio::async_read_until
        /// Forwards results to callback
        void AsyncReadUntil(
            std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback);


        ///
        /// \return Bytes without message terminator, 0 if terminated_bytes < sizeof message terminator
        static std::size_t GetBytesUnterminated(std::size_t terminated_bytes) noexcept;


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


        media::MediaProp mediaProp;

    private:
        Status status_ = Status::send_s_greeting;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_H
