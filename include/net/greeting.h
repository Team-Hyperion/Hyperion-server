// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_GREETING_H
#define HYPERION_INCLUDE_NET_GREETING_H
#pragma once

#include "media/media_prop.h"
#include "net/type_alias.h"

namespace hyperion::net
{
    // Processes greeting (to / from client)

    ///
    /// Information for identifying server with client
    ///
    /// Format:
    /// 0xFE <Terminator>
    [[nodiscard]] ByteVector MakeServerGreeting();


    ///
    /// Parse information for identifying client
    ///
    /// Format:
    /// <Media_type> <Media_width> <Media_height> <Media_fps>
    /// Bytes: 1, 2, 2, 1
    ///
    /// \exception std::runtime_error b_vec has invalid number of bytes
    [[nodiscard]] media::MediaProp ParseClientGreeting(const ByteVector::value_type* ptr, std::size_t size);

    // See above
    [[nodiscard]] media::MediaProp ParseClientGreeting(const ByteVector& b_vec);
} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_GREETING_H
