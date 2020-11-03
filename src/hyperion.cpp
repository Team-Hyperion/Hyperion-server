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

void RunServer(const std::string& save_path, net::NetProp net_prop) {
    asio::io_context io_context;

    media::MediaConfig media_config;
    media_config.mediaSavePath = save_path + std::to_string(std::time(nullptr)); // UNIX time

    LOG_MESSAGE_F(info, "Save directory: %s", media_config.mediaSavePath.c_str());

    net::NetData net_data(std::move(net_prop), std::move(media_config)); // Requires io_context to destruct


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

        TCLAP::ValueArg<std::string> outPathArg("o", "output-path", "Path to save received media from clients",
                                                false, "",
                                                "string");

        cmd.add(outPathArg);


        cmd.parse(argc, argv);


        // Media save path
        std::string save_path = outPathArg.getValue();
        if (!save_path.empty()) {
            save_path.push_back('/');
        }

        // Networking properties


        // Finished parsing command line args, start the server
        RunServer(save_path, {});
    }
    catch (TCLAP::ArgException& e) {
        LOG_MESSAGE_F(error, "%s for CLI arg: %s", e.error(), e.argId());
    }
    catch (std::exception& e) {
        LOG_MESSAGE_F(error, "Error initializing server: %s", e.what());
    }


    LOG_MESSAGE(info, "goodbye!");
    return 0;
}
