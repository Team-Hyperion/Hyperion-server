// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <asio/basic_streambuf.hpp>
#include <asio/buffer.hpp>
#include <asio/io_service.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/streambuf.hpp>
#include <asio/write.hpp>

#include <iostream>

#include "hyperion.h"

#include "core/crash_handler.h"
#include "core/filesystem.h"
#include "core/resource_guard.h"
#include "core/utility.h"

int main(int argc, char* argv[]) {
    using namespace hyperion;

    core::SetExecutingDirectory(argv[0]);

    // Log file
    core::ResourceGuard log_guard(&core::CloseLogFile);
    core::OpenLogFile();

    core::RegisterCrashHandler();

    // Initial startup message
    LOG_MESSAGE_F(info, "%s | %s build, version: %s\n\n", HYPERION_BUILD_TARGET_PLATFORM, BUILD_TYPE, HYPERION_VERSION);

    // Initialize server


    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    // Test only vvv

    asio::io_service io_service;

    // listen for new connection
    asio::ip::tcp::acceptor acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1234));

    // socket creation
    asio::ip::tcp::socket socket(io_service);

    // waiting for connection
    acceptor.accept(socket);


    constexpr char message_terminator = '\n';

    std::string received_msg;
    while (core::StrTrim(received_msg, {'\r', '\n'}) != "Quit") {
        // read operation
        asio::streambuf buf;
        asio::read_until(socket, buf, message_terminator);

        received_msg = asio::buffer_cast<const char*>(buf.data());
        std::cout << received_msg << std::endl;


        // write operation
        const std::string message = "Hello from server!";
        asio::write(socket, asio::buffer(message + message_terminator));
    }
    // Test only ^^^
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //


    LOG_MESSAGE(info, "goodbye!");
    return 0;
}
