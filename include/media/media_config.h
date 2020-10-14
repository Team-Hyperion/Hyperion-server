// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_MEDIA_MEDIA_CONFIG_H
#define HYPERION_INCLUDE_MEDIA_MEDIA_CONFIG_H
#pragma once

#include <limits>
#include <vector>

#include "media/media_prop.h"

namespace hyperion::media
{
    ///
    /// Options which applies to all MediaProp
    struct MediaConfig
    {
        /// Media dimensions which are allowed
        /// Leave empty to allow all dimensions
        std::vector<MediaDimension> dimensions;

        /// Maximum fps for videos
        uint8_t maxFps = std::numeric_limits<uint8_t>::max();

        /// Received media is stored at the folder in this path
        /// E.g "data/media" -> Media saved as data/media/xxx.mp4
        std::string mediaSavePath;
    };
} // namespace hyperion::media

#endif // HYPERION_INCLUDE_MEDIA_MEDIA_CONFIG_H
