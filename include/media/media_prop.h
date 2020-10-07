// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_MEDIA_MEDIA_PROP_H
#define HYPERION_INCLUDE_MEDIA_MEDIA_PROP_H
#pragma once

#include <cstdint>
#include <limits>

namespace hyperion::media
{
    enum class MediaType
    {
        image = 0,
        video,
    };

    class MediaProp
    {
    public:
        MediaProp() = default;

        MediaProp(const MediaType type, const uint16_t width, const uint16_t height, const uint8_t fps)
            : width(width), height(height), fps(fps), type_(static_cast<decltype(type_)>(type)) {}


        [[nodiscard]] MediaType GetType() const {
            return static_cast<MediaType>(type_);
        }
        void SetType(const MediaType type) {
            type_ = static_cast<decltype(type_)>(type);
        }


        uint16_t width  = 0;
        uint16_t height = 0;

        uint8_t fps = 0;

    private:
        uint8_t type_ = std::numeric_limits<decltype(type_)>::max();
    };

} // namespace hyperion::media

#endif // HYPERION_INCLUDE_MEDIA_MEDIA_PROP_H
