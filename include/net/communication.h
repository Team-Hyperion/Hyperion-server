// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_PROCESS_BYTES_H
#define HYPERION_INCLUDE_NET_PROCESS_BYTES_H
#pragma once

#include "media/media_prop.h"
#include "net/type_alias.h"

namespace hyperion::net
{
    // Processes bytes that are (to / from client)

    ///
    /// Format:
    /// <Prefix> <Terminator>
    [[nodiscard]] ByteVector MakeServerGreeting();


    ///
    /// Parses to media::MediaProp
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

#endif // HYPERION_INCLUDE_NET_PROCESS_BYTES_H
