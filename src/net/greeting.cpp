// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/greeting.h"

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
