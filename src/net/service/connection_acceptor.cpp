// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/service/connection_acceptor.h"

#include "core/logger.h"
#include "net/greeting.h"
#include "net/net_data.h"
#include "net/read.h"
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

    Send(socket, serverGreeting_);

    LOG_MESSAGE_F(debug, "Accepted connection %s", socket.remote_endpoint().address().to_string().c_str());


    auto make_connection = [&]() -> Connection& {
        core::GuardedAccess connections(netData_->connections);
        return connections->Add(std::move(socket)); // TODO what if it resizes before it is done reading? max clients?
    };
    auto& conn = make_connection();


    // Get client greeting
    const auto str = ReadTimed(conn, netData_->GetNetProp().toReceiveClientGreeting);

    if (str.empty()) {
        // TODO these 2 may throw
        conn.socket.shutdown(asio::socket_base::shutdown_both);
        conn.socket.close();

        LOG_MESSAGE(debug, "Read client greeting timeout");
    }
    else {
        LOG_MESSAGE(debug, "Received client greeting");
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
