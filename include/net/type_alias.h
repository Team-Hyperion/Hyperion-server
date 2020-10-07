// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_NET_TYPE_ALIAS_H
#define HYPERION_INCLUDE_NET_TYPE_ALIAS_H
#pragma once

#include <vector>

namespace hyperion::net
{
    using ByteVector = std::vector<uint8_t>;
    static_assert(sizeof(ByteVector::value_type) == 1, "ByteVector element should be 1 byte");

} // namespace hyperion::net

#endif // HYPERION_INCLUDE_NET_TYPE_ALIAS_H
