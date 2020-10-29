// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_NET_DATA_H
#define HYPERION_INCLUDE_NET_NET_DATA_H
#pragma once

#include <memory>
#include <utility>

#include "media/media_config.h"
#include "net/connection.h"
#include "net/net_prop.h"

namespace hyperion::net
{
    class ConnectionGc;

    class Connections
    {
        friend ConnectionGc;

    public:
        decltype(auto) Add(Connection::SocketT&& socket) {
            connections_.push_back(std::make_unique<Connection>(std::move(socket)));
            return connections_[connections_.size() - 1];
        }

    private:
        // By heap allocating the Connection, the vector can resize without touching the Connection
        // which is allows for async and threaded actions
        std::vector<std::unique_ptr<Connection>> connections_;
    };


    ///
    /// Top level connection data, shared across connection services
    class NetData
    {
    public:
        NetData(NetProp net_prop, media::MediaConfig media_config)
            : netProp_(std::move(net_prop)), mediaConfig_(std::move(media_config)) {}


        [[nodiscard]] const NetProp& GetNetProp() const {
            return netProp_;
        }

        [[nodiscard]] auto& GetMediaConfig() const {
            return mediaConfig_;
        }

        Connections connections;

        volatile bool servicesExit = false;

    private:
        NetProp netProp_;

        media::MediaConfig mediaConfig_;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_NET_DATA_H
