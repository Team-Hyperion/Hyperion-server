// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/communication.h"

#include <stdexcept>

#include "net/comm_format.h"
#include "net/type_alias.h"

using namespace hyperion;

net::ByteVector net::MakeServerGreeting() {
    ByteVector vec;
    vec.reserve(2);

    vec.push_back(CommFormat::kServerMsgPrefix);
    vec.push_back(CommFormat::kMessageTerminator);

    return vec;
}

media::MediaProp net::ParseClientGreeting(const ByteVector::value_type* ptr, const std::size_t size) {
    if (size < sizeof(media::MediaProp)) {
        throw std::runtime_error("Too few input bytes");
    }

    std::size_t byte_offset = 0;

    auto get_byte   = [&]() -> uint8_t { return ptr[byte_offset++]; };
    auto get_byte_2 = [&]() -> uint16_t { return get_byte() << 8 | get_byte(); };


    static_assert(sizeof(media::MediaProp) == 1 + 2 + 2 + 1);
    const uint8_t type    = get_byte();
    const uint16_t width  = get_byte_2();
    const uint16_t height = get_byte_2();
    const uint8_t fps     = get_byte();

    return {static_cast<media::MediaType>(type), width, height, fps};
}

media::MediaProp net::ParseClientGreeting(const ByteVector& b_vec) {
    return ParseClientGreeting(b_vec.data(), b_vec.size());
}
