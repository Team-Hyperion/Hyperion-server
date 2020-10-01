// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "net/greeting.h"

#include "net/comm_format.h"

std::string hyperion::net::MakeServerGreeting() {
    static_assert(sizeof(std::string::value_type) == 1);


    std::string str;
    str.reserve(2);

    str.push_back(CommFormat::kServerMsgPrefix);
    str.push_back(CommFormat::kMessageTerminator);

    return str;
}
