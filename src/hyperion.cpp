// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "hyperion.h"

#include <ctime>
#include <iostream>
#include <tclap/CmdLine.h>
#include <thread>

#include "core/crash_handler.h"
#include "core/filesystem.h"
#include "core/resource_guard.h"

#include "net/net_data.h"
#include "net/service/connection_acceptor.h"
#include "net/service/connection_gc.h"
#include "net/service/connection_listener.h"

using namespace hyperion;

void RunConnectionServices(asio::io_context& io_context, net::NetData& net_data) {

    // Init all services first to prevent errors between services
    auto conn_acceptor = MakeConnectionAcceptor(net_data, io_context);
    assert(conn_acceptor.has_value());

    auto gc = net::ConnectionGc(io_context, net_data);


    conn_acceptor->onConnectionAccepted = [&net_data](net::Connection& conn) {
        conn.BeginOutFiles(net_data.GetMediaConfig().mediaSavePath);
        BeginAsyncReceive(conn);
    };

    // Start services
    conn_acceptor->BeginAsyncAccept();
    gc.Start();

    io_context.run();
}

///
/// Commands received from the hyperion executable's terminal
/// Blocks until next command is received
void HandleTerminalCommands(net::NetData& net_data) {
    std::string usr_input;
    std::getline(std::cin, usr_input);

    if (usr_input == "stop") {
        net_data.servicesExit = true;
    }
    else {
        LOG_MESSAGE_F(error, "Invalid command %s", usr_input.c_str());
    }
}

void RunServer(const std::string& save_path, const net::NetProp& net_prop) {
    asio::io_context io_context;

    media::MediaConfig media_config;
    media_config.mediaSavePath = save_path + std::to_string(std::time(nullptr)); // UNIX time

    LOG_MESSAGE_F(info, "Save directory: %s", media_config.mediaSavePath.c_str());

    net::NetData net_data(net_prop, std::move(media_config)); // Requires io_context to destruct


    auto conn_services = std::thread(&RunConnectionServices, std::ref(io_context), std::ref(net_data));

    while (!net_data.servicesExit) {
        HandleTerminalCommands(net_data);
    }

    LOG_MESSAGE(info, "Stopping server");
    io_context.stop();

    conn_services.join();
}


int main(const int argc, char* argv[]) {
    core::SetExecutingDirectory(argv[0]);

    // Log file
    core::ResourceGuard log_guard(&core::CloseLogFile);
    core::OpenLogFile();

    core::RegisterCrashHandler();

    // Initial startup message
    LOG_MESSAGE_F(info, "%s | %s build, version: %s\n\n", HYPERION_BUILD_TARGET_PLATFORM, BUILD_TYPE, HYPERION_VERSION);


    try {
        TCLAP::CmdLine cmd("https://github.com/Team-Hyperion/Hyperion-server", ' ', HYPERION_VERSION);

        TCLAP::ValueArg<std::string> out_path_arg(
            "o", "output-path", "Path to save received media from clients", false, "", "string", cmd);

        TCLAP::ValueArg<net::NetProp::PortT> net_port_arg(
            "p", "port", "Networking port to use", false, 34200, "uint", cmd);

        TCLAP::SwitchArg ipv4_switch("", "ipv4", "Use ipv4 protocol, uses ipv6 if false", cmd, false);


        cmd.parse(argc, argv);


        // Media save path
        std::string save_path = out_path_arg.getValue();
        if (!save_path.empty()) {
            save_path.push_back('/');
        }

        // Networking properties
        net::NetProp net_prop;

        net_prop.portNum = net_port_arg.getValue();

        if (ipv4_switch.getValue()) {
            net_prop.netProtocol = net::NetProp::InternetProtocol::v4;
        }
        else {
            net_prop.netProtocol = net::NetProp::InternetProtocol::v6;
        }


        // Finished parsing command line args, start the server
        RunServer(save_path, net_prop);
    }
    catch (TCLAP::ArgException& e) {
        LOG_MESSAGE_F(error, "%s", e.what());
        return 1;
    }
    catch (std::exception& e) {
        LOG_MESSAGE_F(error, "Error initializing server: %s", e.what());
        return 2;
    }


    LOG_MESSAGE(info, "goodbye!");
    return 0;
}
