// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "net/greeting.h"

#include "net/comm_format.h"


namespace hyperion::net
{
    TEST(Greeing, MakeServerGreeting) {
        const auto greeting = MakeServerGreeting();

        ASSERT_EQ(greeting.size(), 2);

        EXPECT_EQ(greeting[0], CommFormat::kServerMsgPrefix);
        EXPECT_EQ(greeting[1], CommFormat::kMessageTerminator);
    }

} // namespace hyperion::net