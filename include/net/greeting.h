// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_GREETING_H
#define HYPERION_INCLUDE_NET_GREETING_H
#pragma once

#include <string>

namespace hyperion::net
{
    // Processes greeting (to / from client)

    ///
    /// Format as follows:
    ///
    /// 0xFE
    [[nodiscard]] std::string MakeServerGreeting();

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_GREETING_H
