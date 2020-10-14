// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/service/connection_listener.h"

#include <asio/error.hpp>
#include <cassert>

#include "core/resource_guard.h"
#include "net/connection.h"
#include "net/logger.h"

using namespace hyperion;

void DoAsyncReceive(net::Connection& conn) {
    assert(conn.buf.size() == 0);

    conn.socket.async_receive( //
        conn.buf.prepare(conn.buf.max_size()),
        [&](const asio::error_code& error, const std::size_t bytes_transferred) {
            core::CapturingGuard<void()> guard([&]() { conn.buf.consume(bytes_transferred); });

            if (error) {
                if (error.value() == asio::error::eof) {
                    conn.End();
                }
                else {
                    NET_LOG_F(error, "Connection listener async receive ec: %s", error.message().c_str());
                }
                return;
            }
            DoAsyncReceive(conn);

            // This stream is unterminated

            // TODO send this to be saved

            //
            //
            //
            //
            //
            //
            // For testing only
            const auto* bytes = asio::buffer_cast<const net::ByteVector::value_type*>(conn.buf.data());
            std::string s;
            s.resize(bytes_transferred);

            for (std::size_t i = 0; i < bytes_transferred; ++i) {
                s[i] = bytes[i];
            }

            LOG_MESSAGE_F(debug, "%llu Received %llu bytes, %s", conn.id, bytes_transferred, s.c_str());
            //
            //
            //
            //
            //
            //
            //
        });
}

void net::BeginAsyncReceive(Connection& connection) {
    DoAsyncReceive(connection);
}
