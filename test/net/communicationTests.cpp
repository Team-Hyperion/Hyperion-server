// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "net/communication.h"

#include "media/media_config.h"
#include "net/comm_format.h"

namespace hyperion::net
{
    TEST(Communication, MakeServerGreeting) {
        const auto greeting = MakeServerGreeting();

        ASSERT_EQ(greeting.size(), 1);

        EXPECT_EQ(greeting[0], CommFormat::kServerMsgPrefix);
    }

    TEST(Communication, ParseClientGreeting) {
        // 1-Video, 2-1920, 2-1080, 1-60
        ByteVector bytes = {0x01, 0x07, 0x80, 0x04, 0x38, 0x3C};
        const auto props = ParseClientGreeting({{{1920, 1080}}, 60}, bytes.data(), bytes.size());

        EXPECT_EQ(props.GetType(), media::MediaType::video);
        EXPECT_EQ(props.width, 1920);
        EXPECT_EQ(props.height, 1080);
        EXPECT_EQ(props.fps, 60);
    }

    TEST(Communication, ParseClientGreetingInvalidMediaType) {
        try {
            // 1-???, 2-1920, 2-1080
            auto props = ParseClientGreeting({{{1920, 1080}}, 60}, {0x03, 0x07, 0x80, 0x04, 0x38, 1});
            FAIL();
        }
        catch (ParseGreetingError&) {
        }
    }

    TEST(Communication, ParseClientGreetingInvalidDimension) {
        try {
            // 1-Image 2-1921, 2-1080
            auto props = ParseClientGreeting({{{1920, 1080}}, 60}, {0x00, 0x07, 0x81, 0x04, 0x38, 1});
            FAIL();
        }
        catch (ParseGreetingError&) {
        }
    }

    TEST(Communication, ParseClientGreetingLowerFps) {
        auto parse = [](const uint8_t media_type_byte, const media::MediaType media_type_literal) {
            // 1-Video, 2-100, 2-101, 1-29
            const auto props = ParseClientGreeting({{{100, 101}}, 30}, {media_type_byte, 0x00, 0x64, 0x00, 0x65, 0x1D});

            EXPECT_EQ(props.GetType(), media_type_literal);
            EXPECT_EQ(props.width, 100);
            EXPECT_EQ(props.height, 101);
            EXPECT_EQ(props.fps, 29);
        };

        parse(0x00, media::MediaType::image);
        parse(0x01, media::MediaType::video);
    }

    TEST(Communication, ParseClientGreetingGreaterFps) {
        auto parse = [](const uint8_t media_type) {
            try {
                // 1-, 2-1920, 2-1080, 1-61
                auto props = ParseClientGreeting({{{1920, 1080}}, 60}, {media_type, 0x07, 0x80, 0x04, 0x38, 0x3D});
                return true;
            }
            catch (ParseGreetingError&) {
                return false;
            }
        };

        EXPECT_TRUE(parse(0));  // Max fps ignored for images
        EXPECT_FALSE(parse(1)); // Max fps enforced for videos
    }

    TEST(Communication, ParseClientGreetingTooFewBytes) {
        try {
            auto props = ParseClientGreeting({{{1920, 1080}}, 60}, {1});
            FAIL();
        }
        catch (ParseGreetingError&) {
        }
    }

    TEST(Communication, ParseClientGreetingTooManyBytes) {
        // Ok, just ignore the excess

        // 1-Image, 2-720, 2-480, 1-30
        const auto props = ParseClientGreeting(
            {{{720, 480}}, 60}, {0x00, 0x02, 0xD0, 0x01, 0xE0, 0x1E, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});

        EXPECT_EQ(props.GetType(), media::MediaType::image);
        EXPECT_EQ(props.width, 720);
        EXPECT_EQ(props.height, 480);
        EXPECT_EQ(props.fps, 30);
    }
} // namespace hyperion::net