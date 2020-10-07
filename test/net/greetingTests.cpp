// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "net/greeting.h"

#include "net/comm_format.h"

namespace hyperion::net
{
    TEST(Greeting, MakeServer) {
        const auto greeting = MakeServerGreeting();

        ASSERT_EQ(greeting.size(), 2);

        EXPECT_EQ(greeting[0], CommFormat::kServerMsgPrefix);
        EXPECT_EQ(greeting[1], CommFormat::kMessageTerminator);
    }

    TEST(Greeting, ParseClient) {
        // 1-Video, 2-1920, 2-1080, 1-60
        ByteVector bytes = {0x01, 0x07, 0x80, 0x04, 0x38, 0x3C};
        const auto props = ParseClientGreeting(bytes.data(), bytes.size());

        EXPECT_EQ(props.GetType(), media::MediaType::video);
        EXPECT_EQ(props.width, 1920);
        EXPECT_EQ(props.height, 1080);
        EXPECT_EQ(props.fps, 60);
    }

    TEST(Greeting, ParseClientTooFewBytes) {
        try {
            auto props = ParseClientGreeting({1});
            FAIL();
        }
        catch (std::exception&) {
        }
    }

    TEST(Greeting, ParseClientTooManyBytes) {
        // Ok, just ignore the excess

        // 1-Image, 2-720, 2-480, 1-30
        const auto props =
            ParseClientGreeting({0x00, 0x02, 0xD0, 0x01, 0xE0, 0x1E, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});

        EXPECT_EQ(props.GetType(), media::MediaType::image);
        EXPECT_EQ(props.width, 720);
        EXPECT_EQ(props.height, 480);
        EXPECT_EQ(props.fps, 30);
    }
} // namespace hyperion::net