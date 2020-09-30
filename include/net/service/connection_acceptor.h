// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
#define HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
#pragma once

#include <asio/ip/tcp.hpp>
#include <memory>

namespace hyperion::net
{
    class NetData;

    ///
    /// Accepts connections
    class ConnectionAcceptor
    {
    public:
        ///
        /// \exception exception Failed to initialize
        explicit ConnectionAcceptor(asio::io_context& io_context, NetData& net_data);

        ///
        /// Returns immediately, accepts connections until io_context stops
        void BeginAsyncAccept() noexcept;

    private:
        ///
        /// Returns immediately, initializes async accept
        void DoAsyncAccept();

        ///
        /// Called when a connection was accepted
        void HandleAccept(const asio::error_code& error, asio::ip::tcp::socket& socket) const;


        asio::ip::tcp::acceptor acceptor_;
        NetData* netData_;
    };


    ///
    /// Blocks until successfully made ConnectionAcceptor or servicesExit is true
    /// \return nullptr if servicesExit is true and no ConnectionAcceptor was successfully made
    [[nodiscard]] std::unique_ptr<ConnectionAcceptor> MakeConnectionAcceptor(NetData& net_data,
                                                                             asio::io_context& io_context) noexcept;
} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_SERVICE_CONNECTION_ACCEPTOR_H
