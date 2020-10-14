// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_MEDIA_MEDIA_CONFIG_H
#define HYPERION_INCLUDE_MEDIA_MEDIA_CONFIG_H
#pragma once

#include <vector>

#include "media/media_prop.h"

namespace hyperion::media
{
    ///
    /// Options which applies to all MediaProp
    struct MediaConfig
    {
        MediaConfig(std::vector<MediaDimension> dimensions, const uint8_t max_fps)
            : dimensions(std::move(dimensions)), maxFps(max_fps) {}


        /// Media dimensions which are allowed
        std::vector<MediaDimension> dimensions;

        /// Maximum fps for videos
        uint8_t maxFps;
    };
} // namespace hyperion::media

#endif // HYPERION_INCLUDE_MEDIA_MEDIA_CONFIG_H
