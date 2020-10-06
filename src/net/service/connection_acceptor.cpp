// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/service/connection_acceptor.h"

#include "asio.hpp"
#include "core/logger.h"
#include "net/greeting.h"
#include "net/net_data.h"
#include "net/send.h"

using namespace hyperion;

net::ConnectionAcceptor::ConnectionAcceptor(asio::io_context& io_context, NetData& net_data, std::string greeting)
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

void net::ConnectionAcceptor::DoAsyncAccept() {
    acceptor_.async_accept([&](const asio::error_code& error, asio::ip::tcp::socket socket) {
        DoAsyncAccept();
        HandleAccept(error, socket);
    });
}

void net::ConnectionAcceptor::HandleAccept(const asio::error_code& error, asio::ip::tcp::socket& socket) const {
    if (error) {
        LOG_MESSAGE_F(error, "Failed to accept client %s", error.message().c_str());
        return;
    }

    auto make_connection = [&]() -> Connection& {
        core::GuardedAccess connections(netData_->connections);
        return *connections->Add(std::move(socket));
    };
    auto& conn = make_connection(); // Heap allocated, exists after this method exits

    async_write(conn.socket,
                asio::buffer(serverGreeting_),
                [&](const asio::error_code& aw_error, std::size_t /*bytes_transferred*/) {
                    if (aw_error) {
                        LOG_MESSAGE_F(error, "Failed send server greeting: %s", aw_error.message().c_str());
                        return;
                    }

                    CallbackSentGreeting(conn);
                });
}

// Callbacks

void net::ConnectionAcceptor::CallbackSentGreeting(Connection& conn) const noexcept {
    LOG_MESSAGE_F(debug, "Accepted connection %s", conn.socket.remote_endpoint().address().to_string().c_str());

    // Get client greeting
    conn.AsyncReadUntil([&](const asio::error_code& error, const std::size_t /*bytes_transferred*/) {
        if (error) {
            LOG_MESSAGE_F(error, "Failed receiving client greeting: %s", error.message().c_str());
            return;
        }

        CallbackReceivedGreeting(conn);
    });

    // Timeout for receiving greeting
    conn.timer.expires_after(netData_->GetNetProp().toReceiveClientGreeting);
    conn.timer.async_wait([&](const asio::error_code& error) {
        if (error) {
            LOG_MESSAGE_F(error, "Failed client greeting timeout: %s", error.message().c_str());
            return;
        }

        CallbackReceiveGreetingTimeout(conn);
    });
}

void net::ConnectionAcceptor::CallbackReceivedGreeting(Connection& conn) const noexcept {
    conn.SetStatus(Connection::Status::active);
    // TODO parse client greeting
}

void net::ConnectionAcceptor::CallbackReceiveGreetingTimeout(Connection& conn) const noexcept {
    if (conn.GetStatus() != Connection::Status::awaiting_c_greeting) {
        return;
    }

    LOG_MESSAGE(debug, "Receive client greeting timeout");

    try {
        conn.socket.shutdown(asio::socket_base::shutdown_both);
        conn.socket.close();
    }
    catch (std::exception& e) {
        LOG_MESSAGE_F(error, "Failed to close socket: %s", e.what());
    }
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
