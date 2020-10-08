// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
#define HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
#pragma once

#include <asio/ip/tcp.hpp>
#include <optional>

#include "net/type_alias.h"

namespace hyperion::net
{
    class Connection;
    class NetData;

    ///
    /// Accepts connections
    class ConnectionAcceptor
    {
    public:
        ///
        /// \param greeting Sent to clients when connection is accepted
        /// \exception exception Failed to initialize
        explicit ConnectionAcceptor(asio::io_context& io_context, NetData& net_data, ByteVector greeting);

        ///
        /// Returns immediately, accepts connections until io_context stops
        void BeginAsyncAccept() noexcept;

    private:
        ///
        /// Returns immediately, initializes async accept
        void DoAsyncAccept();

        ///
        /// Sets up callbacks for sending greeting, listening for greeting, timing out greeting listen
        void HandleAccept(const asio::error_code& error, asio::ip::tcp::socket& socket) const;

        // Callbacks used in HandleAccept

        ///
        /// Server greeting was successfully sent
        void CallbackSentGreeting(Connection& conn) const noexcept;

        ///
        /// Client greeting was successfully received
        static void CallbackReceivedGreeting(Connection& conn, std::size_t bytes_transferred) noexcept;

        // Error handling

        ///
        /// Server greeting was NOT successfully sent in time
        static void CallbackSendGreetingTimeout(Connection& conn) noexcept;

        ///
        /// Client greeting was NOT successfully received in time
        static void CallbackReceiveGreetingTimeout(Connection& conn) noexcept;


        asio::ip::tcp::acceptor acceptor_;
        NetData* netData_;

        ByteVector serverGreeting_; // Cached
    };


    ///
    /// Blocks until successfully made ConnectionAcceptor or servicesExit is true
    /// \return nullptr if servicesExit is true and no ConnectionAcceptor was successfully made
    [[nodiscard]] std::optional<ConnectionAcceptor> MakeConnectionAcceptor(NetData& net_data,
                                                                           asio::io_context& io_context) noexcept;
} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
