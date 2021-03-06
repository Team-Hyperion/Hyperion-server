// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/communication.h"

#include <algorithm>

#include "core/logger.h"
#include "media/media_config.h"
#include "net/comm_format.h"
#include "net/type_alias.h"

using namespace hyperion;

net::ByteVector net::MakeServerGreeting() {
    ByteVector vec;
    vec.reserve(1);

    vec.push_back(CommFormat::kServerMsgPrefix);

    return vec;
}

media::MediaProp net::ParseClientGreeting(const media::MediaConfig& config,
                                          const ByteVector::value_type* ptr,
                                          const std::size_t size) {
    if (size < sizeof(media::MediaProp)) {
        throw ParseGreetingError("Too few input bytes");
    }

    std::size_t byte_offset = 0;

    auto get_byte   = [&]() -> uint8_t { return ptr[byte_offset++]; };
    auto get_byte_2 = [&]() -> uint16_t { return get_byte() << 8 | get_byte(); };


    static_assert(sizeof(media::MediaProp) == 1 + 2 + 2 + 1);


    const uint8_t type    = get_byte();
    const auto media_type = static_cast<media::MediaType>(type);

    LOG_MESSAGE_F(debug, "Received media type %hhu", type);

    if (type >= static_cast<uint8_t>(media::MediaType::count_)) {
        throw ParseGreetingError("Invalid media type");
    }


    const uint16_t width  = get_byte_2();
    const uint16_t height = get_byte_2();

    LOG_MESSAGE_F(debug, "Received media type %hux%hu", width, height);

    if (!config.dimensions.empty() &&
        std::find(config.dimensions.begin(), config.dimensions.end(), media::MediaDimension(width, height)) ==
            config.dimensions.end()) {
        throw ParseGreetingError("Unsupported media dimension");
    }


    const uint8_t fps = get_byte();

    LOG_MESSAGE_F(debug, "Received fps %hhu", fps);

    if (media_type == media::MediaType::video) {
        if (fps == 0) {
            throw ParseGreetingError("FPS cannot be 0");
        }
        if (fps > config.maxFps) {
            throw ParseGreetingError("FPS greater than maximum");
        }
    }

    return {media_type, width, height, fps};
}

media::MediaProp net::ParseClientGreeting(const media::MediaConfig& config, const ByteVector& b_vec) {
    return ParseClientGreeting(config, b_vec.data(), b_vec.size());
}
