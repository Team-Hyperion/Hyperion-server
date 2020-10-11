// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_NET_DATA_H
#define HYPERION_INCLUDE_NET_NET_DATA_H
#pragma once

#include <memory>
#include <utility>

#include "core/guarded_data.h"

#include "net/connection.h"
#include "net/net_prop.h"

namespace hyperion::net
{
    class Connections
    {
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
        NetData(NetProp net_prop, std::vector<media::MediaDimension> allowed_dimensions)
            : netProp_(std::move(net_prop)), allowedDimensions_(std::move(allowed_dimensions)) {}


        [[nodiscard]] const NetProp& GetNetProp() const {
            return netProp_;
        }

        [[nodiscard]] auto& GetAllowedMediaDim() const {
            return allowedDimensions_;
        }

        core::GuardedData<Connections> connections;

        volatile bool servicesExit = false;

    private:
        NetProp netProp_;

        std::vector<media::MediaDimension> allowedDimensions_;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_NET_DATA_H
