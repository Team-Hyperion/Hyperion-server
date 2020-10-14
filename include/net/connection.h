// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_CONNECTION_H
#define HYPERION_INCLUDE_NET_CONNECTION_H
#pragma once

#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>
#include <asio/streambuf.hpp>
#include <functional>

#include "media/media_prop.h"
#include "net/type_alias.h"

namespace hyperion::net
{
    enum class ConnectionStatus
    {
        send_s_greeting,
        awaiting_c_greeting,
        active // Client greeting received and accepted
    };


    ///
    /// Connection without networking parts for testing
    class ConnectionBase
    {
    public:
        [[nodiscard]] ConnectionStatus GetStatus() const noexcept {
            return status_;
        }

        void SetStatus(const ConnectionStatus status) noexcept {
            // Can only move down the list of ConnectionStatus
            assert(static_cast<unsigned>(status) >= static_cast<unsigned>(status_));

            this->status_ = status;
        }


        media::MediaProp mediaProp;

    private:
        ConnectionStatus status_ = ConnectionStatus::send_s_greeting;
    };


    ///
    /// A connection with a client
    class Connection : public ConnectionBase
    {
        using IdT = std::size_t;

        inline static IdT nextConnectionId_ = 1;

    public:
        /// Bytes
        static constexpr auto kReceiveBufSize = 1000;

        using SocketT = asio::ip::tcp::socket;


        explicit Connection(SocketT&& socket);

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
        /// Wrapper around asio::async_read
        /// \param n Bytes to read
        /// \param callback Results forwarded to
        /// \remark callback must consume bytes_transferred from buf
        void AsyncRead(std::size_t n,
                       std::function<void(const asio::error_code& error, std::size_t bytes_transferred)>&& callback);


        const IdT id;

        SocketT socket;
        asio::streambuf buf{kReceiveBufSize};

        /// Use for timeouts
        asio::steady_timer timer;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_H
