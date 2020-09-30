// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/service/connection_acceptor.h"

#include <thread>

#include "core/logger.h"

#include "net/net_data.h"

using namespace hyperion;

net::ConnectionAcceptor::ConnectionAcceptor(asio::io_context& io_context, NetData& net_data)
    : acceptor_(io_context, asio::ip::tcp::endpoint()), netData_(&net_data) {

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
    try {
        acceptor_.async_accept([&](const asio::error_code& error, asio::ip::tcp::socket socket) {
            HandleAccept(error, socket);
            DoAsyncAccept();
        });
    }
    catch (std::exception& e) {
        LOG_MESSAGE_F(error, "Failed to accept client %s", e.what());
        std::this_thread::sleep_for(netData_->GetNetProp().sleepAcceptorAcceptClientFailed);
    }
}

void net::ConnectionAcceptor::HandleAccept(const asio::error_code& error, asio::ip::tcp::socket& socket) const {
    if (error) {
        throw std::runtime_error(error.message().c_str());
    }

    socket.send(asio::buffer("Hello from server")); // TODO temp greeting

    LOG_MESSAGE_F(debug, "Accepted connection %s", socket.remote_endpoint().address().to_string().c_str());

    core::GuardedAccess access(netData_->connections);
    access->emplace_back(std::move(socket));
}

//

std::unique_ptr<net::ConnectionAcceptor> net::MakeConnectionAcceptor(NetData& net_data,
                                                                     asio::io_context& io_context) noexcept {
    while (!net_data.servicesExit) {
        try {
            return std::make_unique<ConnectionAcceptor>(io_context, net_data);
        }
        catch (std::exception& e) {
            LOG_MESSAGE_F(error, "Failed to initialize connection acceptor %s", e.what());
            std::this_thread::sleep_for(net_data.GetNetProp().sleepAcceptorInitFailed);
        }
    }

    return nullptr;
}
