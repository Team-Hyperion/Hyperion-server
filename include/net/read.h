// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_READ_H
#define HYPERION_INCLUDE_NET_READ_H
#pragma once

#include <asio/ip/tcp.hpp>

#include "net/timed_action.h"

namespace hyperion::net
{
    class Connection;

    ///
    /// \remark May still read into connection's buffer after timeout
    [[nodiscard]] std::string ReadTimed(Connection& conn, const TimedAction::DurationT& timeout);
} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_READ_H
