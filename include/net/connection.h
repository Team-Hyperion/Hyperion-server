// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_CONNECTION_H
#define HYPERION_INCLUDE_NET_CONNECTION_H
#pragma once

#include <asio/ip/tcp.hpp>
#include <asio/steady_timer.hpp>
#include <asio/streambuf.hpp>
#include <fstream>
#include <functional>
#include <string>

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
        using IdT = std::size_t;

        inline static IdT nextConnectionId_ = 1;

    public:
        ConnectionBase() : id(nextConnectionId_++) {}

        [[nodiscard]] ConnectionStatus GetStatus() const noexcept {
            return status_;
        }

        void SetStatus(const ConnectionStatus status) noexcept {
            // Can only move down the list of ConnectionStatus
            assert(static_cast<unsigned>(status) >= static_cast<unsigned>(status_));

            this->status_ = status;
        }


        ///
        /// Call ONCE before any methods involving out files
        void BeginOutFiles(std::string file_directory_path);

        ///
        /// Opens and returns ofstream to file for saving media to
        /// Creates out file if non existent
        [[nodiscard]] std::ofstream& OpenOutFile();

        ///
        /// Forever done writing to the file
        /// Next call to OpenOutFile will create a new file
        void FinishOutFile();


        const IdT id;

        media::MediaProp mediaProp;

    private:
        [[nodiscard]] std::string MakeOutFilePath() const;

        ConnectionStatus status_ = ConnectionStatus::send_s_greeting;

        std::string outFilePath_;
        std::string outFileDirectory_;

        std::ofstream outFile_;
    };


    ///
    /// A connection with a client
    class Connection : public ConnectionBase
    {

    public:
        /// Bytes
        static constexpr auto kReceiveBufSize = 32768;

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


        SocketT socket;
        asio::streambuf buf{kReceiveBufSize};

        /// Use for timeouts
        asio::steady_timer timer;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_CONNECTION_H
