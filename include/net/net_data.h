// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_NET_DATA_H
#define HYPERION_INCLUDE_NET_NET_DATA_H
#pragma once

#include "core/guarded_data.h"

#include "net/connection.h"
#include "net/net_prop.h"

namespace hyperion::net
{
    ///
    /// Top level connection data, shared across connection services
    class NetData
    {

    public:
        [[nodiscard]] const NetProp& GetNetProp() const {
            return netProp_;
        }

        core::GuardedData<std::vector<Connection>> connections;

        volatile bool servicesExit = false;

    private:
        NetProp netProp_;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_NET_DATA_H
