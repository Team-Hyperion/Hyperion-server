// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/service/connection_listener.h"

#include <asio/error.hpp>
#include <cassert>

#include "core/resource_guard.h"
#include "net/connection.h"
#include "net/logger.h"

using namespace hyperion;

void DoAsyncReceive(net::Connection& conn) {
    assert(conn.GetStreambuf().size() == 0);

    conn.AsyncReceive(
        conn.GetStreambuf().max_size(), [&](const asio::error_code& error, const std::size_t bytes_transferred) {
            core::CapturingGuard<void()> guard([&]() { conn.GetStreambuf().consume(bytes_transferred); });

            if (error) {
                if (error.value() != asio::error::eof) {
                    NET_LOG_F(error, "Connection listener async receive ec: %s", error.message().c_str());
                }
                conn.End();
                return;
            }

            // This stream is unterminated
            LOG_MESSAGE_F(debug, "%llu Received %llu bytes", conn.id, bytes_transferred);

            const auto* bytes = asio::buffer_cast<const net::ByteVector::value_type*>(conn.GetStreambuf().data());

            try {
                auto& out_file = conn.OpenOutFile();
                core::CapturingGuard<void()> of_guard([&]() { out_file.close(); });

                for (std::size_t i = 0; i < bytes_transferred; ++i) {
                    out_file << bytes[i];
                }
            }
            catch (std::exception& e) {
                LOG_MESSAGE_F(error, "Failed to save received bytes to file: %s", e.what());
            }

            DoAsyncReceive(conn); // This needs to be at the END after done processing received bytes!!
        });
}

void net::BeginAsyncReceive(Connection& connection) {
    DoAsyncReceive(connection);
}
