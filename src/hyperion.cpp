// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "hyperion.h"

#include <iostream>
#include <thread>

#include "core/crash_handler.h"
#include "core/filesystem.h"
#include "core/resource_guard.h"

#include "net/net_data.h"
#include "net/service/connection_acceptor.h"


using namespace hyperion;

void RunConnectionServices(asio::io_context& io_context, net::NetData& net_data) {

    // Init all services first to prevent errors between services
    auto conn_acceptor = MakeConnectionAcceptor(net_data, io_context);
    assert(conn_acceptor.has_value());


    // Start services
    conn_acceptor->BeginAsyncAccept();


    // constexpr char message_terminator = '\n';
    //
    // std::string received_msg;
    // while (core::StrTrim(received_msg, {'\r', '\n'}) != "Quit") {
    //     // read operation
    //     asio::streambuf buf;
    //     asio::read_until(socket, buf, message_terminator);
    //
    //     received_msg = asio::buffer_cast<const char*>(buf.data());
    //     std::cout << received_msg << std::endl;
    //
    //
    //     // write operation
    //     const std::string message = "Hello from server!";
    //     asio::write(socket, asio::buffer(message + message_terminator));
    // }

    io_context.run();
}

void RunServer() {
    asio::io_context io_context;
    net::NetData net_data; // Requires io_context to destruct


    auto conn_services = std::thread(&RunConnectionServices, std::ref(io_context), std::ref(net_data));

    // Stop the server with user input "stop"
    std::string usr_input;
    while (usr_input != "stop") {
        std::cin >> usr_input;
    }

    LOG_MESSAGE(info, "Stopping server");
    net_data.servicesExit = true;
    io_context.stop();

    conn_services.join();
}


int main(int argc, char* argv[]) {
    core::SetExecutingDirectory(argv[0]);

    // Log file
    core::ResourceGuard log_guard(&core::CloseLogFile);
    core::OpenLogFile();

    core::RegisterCrashHandler();

    // Initial startup message
    LOG_MESSAGE_F(info, "%s | %s build, version: %s\n\n", HYPERION_BUILD_TARGET_PLATFORM, BUILD_TYPE, HYPERION_VERSION);


    // Initialize server
    RunServer();


    LOG_MESSAGE(info, "goodbye!");
    return 0;
}
