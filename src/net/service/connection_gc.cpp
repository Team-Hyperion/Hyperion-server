// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/service/connection_gc.h"

#include <thread>

#include "core/logger.h"
#include "net/logger.h"
#include "net/net_data.h"

using namespace hyperion;

void net::ConnectionGc::Start() {
    SetupSweepTimer();
}

void net::ConnectionGc::SetupSweepTimer() {
    timer_.expires_after(netData_.GetNetProp().sleepGcPass);
    timer_.async_wait([&](const asio::error_code& error) {
        if (error) {
            NET_LOG_F(error, "GC timer ec: %s", error.message().c_str());
        }

        Sweep(netData_);
        SetupSweepTimer();
    });
}

void net::ConnectionGc::Sweep(NetData& net_data) {
    LOG_MESSAGE(debug, "Beginning connection gc pass");

    auto& connections = net_data.connections.connections_;

    // Must use iterators as this will be erasing at it iterates
    for (auto it = connections.begin(); it != connections.end();) {
        auto& conn = *it;

        if (conn->CanDestruct()) {
            LOG_MESSAGE_F(debug, "Sweeping connection %llu", conn->id);
            it = connections.erase(it);
        }
        else {
            ++it;
        }
    }
}
