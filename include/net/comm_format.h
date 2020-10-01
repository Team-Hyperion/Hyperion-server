// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_COMM_FORMAT_H
#define HYPERION_INCLUDE_NET_COMM_FORMAT_H
#pragma once

namespace hyperion::net
{
    // Defines globals for communication standards

    class CommFormat
    {
    public:
        static constexpr auto kMessageTerminator = '\0';
        static constexpr char kServerMsgPrefix   = '\xFE';
    };

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_COMM_FORMAT_H
