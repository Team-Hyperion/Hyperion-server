// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/read.h"

#include <asio/read_until.hpp>

#include "net/comm_format.h"
#include "net/connection.h"

using namespace hyperion;

std::string net::ReadTimed(Connection& conn, const TimedAction::DurationT& timeout) {
    std::string str;

    TimedAction action(timeout, [&]() {
        async_read_until(conn.socket,
                         *conn.buf,
                         CommFormat::kMessageTerminator,
                         [&](const asio::error_code& error, const std::size_t bytes_transferred) {
                             if (error) {
                                 LOG_MESSAGE_F(error, "Timed async read error: %s", error.message().c_str());
                                 return;
                             }

                             LOG_MESSAGE_F(debug, "Read %lld bytes", bytes_transferred);

                             // Buffer may contain additional data past bytes_transferred which does not belong to me
                             static_assert(sizeof(char) == 1);

                             str.reserve(bytes_transferred);
                             for (std::size_t i = 0; i < bytes_transferred; ++i) {
                                 str[i] = asio::buffer_cast<const char*>(conn.buf->data())[i];
                             }
                         });
    });

    action.Start();

    return str;
}
