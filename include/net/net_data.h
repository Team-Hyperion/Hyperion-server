// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_NET_DATA_H
#define HYPERION_INCLUDE_NET_NET_DATA_H
#pragma once

#include "core/guarded_data.h"
#include "net/net_prop.h"

namespace hyperion::net
{
    ///
    /// Top level connection data, shared across connection services
    class NetData
    {

    public:
        core::GuardedData<NetProp> netProp;

        volatile bool servicesExit = false;
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_NET_DATA_H
