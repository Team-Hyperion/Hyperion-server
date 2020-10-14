// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/service/connection_acceptor.h"

#include <asio/ip/tcp.hpp>

#include "core/logger.h"
#include "core/resource_guard.h"
#include "net/communication.h"
#include "net/logger.h"
#include "net/net_data.h"

using namespace hyperion;

net::ConnectionAcceptor::ConnectionAcceptor(asio::io_context& io_context, NetData& net_data, ByteVector greeting)
    : acceptor_(io_context, asio::ip::tcp::endpoint()), netData_(&net_data), serverGreeting_(std::move(greeting)) {

    assert(netData_ != nullptr);

    const auto& net_prop = netData_->GetNetProp();

    asio::ip::tcp protocol    = asio::ip::tcp::v4();
    const char* protocol_cstr = "Unknown protocol";

    switch (net_prop.netProtocol) {
    case NetProp::InternetProtocol::v4:
        protocol      = asio::ip::tcp::v4();
        protocol_cstr = "v4";
        break;
    case NetProp::InternetProtocol::v6:
        protocol      = asio::ip::tcp::v6();
        protocol_cstr = "v6";
        break;
    }

    LOG_MESSAGE_F(info, "Initializing connection acceptor. Protocol: %s, Port: %d", protocol_cstr, net_prop.portNum);

    acceptor_ = asio::ip::tcp::acceptor{io_context, asio::ip::tcp::endpoint(protocol, net_prop.portNum)};
}

void net::ConnectionAcceptor::BeginAsyncAccept() noexcept {
    LOG_MESSAGE(info, "Starting connection acceptor");
    DoAsyncAccept();
}

void net::ConnectionAcceptor::DoAsyncAccept() noexcept {
    try {
        acceptor_.async_accept([&](const asio::error_code& error, asio::ip::tcp::socket socket) {
            DoAsyncAccept();

            if (!error) {
                HandleAccept(socket);
            }
            else {
                LOG_MESSAGE_F(error, "Failed to accept client %s", error.message().c_str());
            }
        });
    }
    catch (std::exception& e) {
        LOG_MESSAGE_F(error, "Failed to setup async accept, no new connections will be accepted: %s", e.what());
    }
}

void net::ConnectionAcceptor::HandleAccept(asio::ip::tcp::socket& socket) const {
    auto make_connection = [&]() -> Connection& {
        core::GuardedAccess connections(netData_->connections);
        return *connections->Add(std::move(socket));
    };
    auto& conn = make_connection(); // Heap allocated, exists after this method exits


    // Send server greeting
    conn.AsyncWrite(serverGreeting_, [&](const asio::error_code& error, std::size_t /*bytes_transferred*/) {
        if (error) {

            NET_LOG_F(error, "Send server greeting ec: %s", error.message().c_str());
            return;
        }

        CallbackSentGreeting(conn);
    });

    // Timeout
    conn.timer.expires_after(netData_->GetNetProp().toSendServerGreeting);
    conn.timer.async_wait([&](const asio::error_code& error) {
        if (error) {
            NET_LOG_F(error, "Send server greeting timeout ec: %s", error.message().c_str());
            return;
        }

        CallbackSendGreetingTimeout(conn);
    });
}

// Callbacks

void net::ConnectionAcceptor::CallbackSentGreeting(Connection& conn) const noexcept {
    try {
        // Get client greeting
        conn.AsyncRead(kParseClientGreetingBytes,
                       [&](const asio::error_code& error, const std::size_t bytes_transferred) {
                           if (error) {
                               NET_LOG_F(error, "Receiving client greeting ec: %s", error.message().c_str());
                               return;
                           }

                           CallbackReceivedGreeting(conn, bytes_transferred);
                       });

        // Timeout
        conn.timer.expires_after(netData_->GetNetProp().toReceiveClientGreeting);
        conn.timer.async_wait([&](const asio::error_code& error) {
            if (error) {
                NET_LOG_F(error, "Received client greeting timeout ec: %s", error.message().c_str());
                return;
            }

            CallbackReceiveGreetingTimeout(conn);
        });

        conn.SetStatus(ConnectionStatus::awaiting_c_greeting);
    }
    catch (std::exception& e) {
        LOG_MESSAGE_F(error, "Callback sent greeting: %s", e.what());
        conn.End();
    }
}

void net::ConnectionAcceptor::CallbackReceivedGreeting(Connection& conn,
                                                       const std::size_t bytes_transferred) const noexcept {
    try {
        core::CapturingGuard<void()> guard([&]() { conn.buf.consume(bytes_transferred); });
        const auto* bytes = asio::buffer_cast<const ByteVector::value_type*>(conn.buf.data());

        conn.mediaProp = ParseClientGreeting(netData_->GetMediaConfig(), bytes, bytes_transferred);


        conn.SetStatus(ConnectionStatus::active);

        try {
            assert(conn.buf.size() == 0); // Buffer should be empty after finishing acceptance

            LOG_MESSAGE_F(info, "Accepted connection %llu", conn.id);
            onConnectionAccepted(conn);
        }
        catch (std::exception& e) {
            LOG_MESSAGE_F(error, "Failed calling callback for connection accepted: %s", e.what());

            conn.End();
        }
    }
    catch (std::exception& e) {
        LOG_MESSAGE_F(debug, "Failed to parse client provided greeting: %s", e.what());

        conn.End();
    }
}

void net::ConnectionAcceptor::CallbackSendGreetingTimeout(Connection& conn) noexcept {
    if (conn.GetStatus() != ConnectionStatus::send_s_greeting) {
        return;
    }

    LOG_MESSAGE(debug, "Send client greeting timeout");

    conn.End();
}

void net::ConnectionAcceptor::CallbackReceiveGreetingTimeout(Connection& conn) noexcept {
    assert(conn.GetStatus() != ConnectionStatus::send_s_greeting); // Server greeting should have been send first

    if (conn.GetStatus() != ConnectionStatus::awaiting_c_greeting) {
        return;
    }

    LOG_MESSAGE(debug, "Receive client greeting timeout");

    conn.End();
}

//

std::optional<net::ConnectionAcceptor> net::MakeConnectionAcceptor(NetData& net_data,
                                                                   asio::io_context& io_context) noexcept {
    while (!net_data.servicesExit) {
        try {
            return std::make_optional<ConnectionAcceptor>(io_context, net_data, MakeServerGreeting());
        }
        catch (std::exception& e) {
            LOG_MESSAGE_F(error, "Failed to initialize connection acceptor %s", e.what());
            asio::steady_timer t(io_context, net_data.GetNetProp().sleepAcceptorInitFailed);
        }
    }

    return std::optional<ConnectionAcceptor>();
}
