// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "net/connection.h"

#include <fstream>

namespace hyperion::net
{
    TEST(Connection, SetStatus) {
        // Moving back up the list of statuses

        ConnectionBase c;

        c.SetStatus(ConnectionStatus::awaiting_c_greeting);
        EXPECT_EQ(c.GetStatus(), ConnectionStatus::awaiting_c_greeting);

        c.SetStatus(ConnectionStatus::active);
        EXPECT_EQ(c.GetStatus(), ConnectionStatus::active);
    }
} // namespace hyperion::net