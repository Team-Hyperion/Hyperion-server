// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "net/communication.h"

#include "media/media_config.h"
#include "net/comm_format.h"

namespace hyperion::net
{
    class CommunicationTest : public testing::Test
    {
    protected:
        [[nodiscard]] static ByteVector MakeClientGreeting(media::MediaType media_type,
                                                           const uint16_t width,
                                                           const uint16_t height,
                                                           const uint8_t fps) {
            ByteVector v;
            v.reserve(6);
            v.push_back(static_cast<uint8_t>(media_type));

            v.push_back(static_cast<uint8_t>(width >> 8));
            v.push_back(static_cast<uint8_t>(width << 8 >> 8));

            v.push_back(static_cast<uint8_t>(height >> 8));
            v.push_back(static_cast<uint8_t>(height << 8 >> 8));

            v.push_back(fps);

            return v;
        }
    };


    TEST_F(CommunicationTest, MakeServerGreeting) {
        const auto greeting = MakeServerGreeting();

        ASSERT_EQ(greeting.size(), 1);

        EXPECT_EQ(greeting[0], CommFormat::kServerMsgPrefix);
    }


    TEST_F(CommunicationTest, ParseClientGreeting) {
        auto bytes       = MakeClientGreeting(media::MediaType::video, 1920, 1080, 60);
        const auto props = ParseClientGreeting({{{1920, 1080}}, 60}, bytes.data(), bytes.size());

        EXPECT_EQ(props.GetType(), media::MediaType::video);
        EXPECT_EQ(props.width, 1920);
        EXPECT_EQ(props.height, 1080);
        EXPECT_EQ(props.fps, 60);
    }

    TEST_F(CommunicationTest, ParseClientGreetingInvalidMediaType) {
        auto parse = [](uint8_t invalid_media_type) {
            try {
                auto props = ParseClientGreeting(
                    {{{1920, 1080}}, 60},
                    MakeClientGreeting(static_cast<media::MediaType>(invalid_media_type), 1920, 1080, 60));
                FAIL();
            }
            catch (ParseGreetingError&) {
            }
        };

        parse(3);
        parse(10);
        parse(30);
        parse(255);
    }

    TEST_F(CommunicationTest, ParseClientGreetingInvalidDimension) {
        auto parse = [](std::vector<media::MediaDimension> allowed, const uint16_t width, const uint16_t height) {
            try {
                auto props = ParseClientGreeting({std::move(allowed), 30},
                                                 MakeClientGreeting(media::MediaType::image, width, height, 30));
                FAIL();
            }
            catch (ParseGreetingError&) {
            }
        };

        parse({}, 0, 0);

        parse({{1920, 1080}}, 1921, 1080);
        parse({{1920, 1080}}, 1920, 1081);

        parse({{1, 2}, {3, 4}}, 2, 1);
        parse({{1, 2}, {3, 4}}, 4, 3);
    }

    TEST_F(CommunicationTest, ParseClientGreetingFps) {
        auto parse = [](const media::MediaType media_type, const uint8_t fps) {
            try {
                const auto props =
                    ParseClientGreeting({{{100, 101}}, 30}, MakeClientGreeting(media_type, 100, 101, fps));

                EXPECT_EQ(props.GetType(), media_type);
                EXPECT_EQ(props.width, 100);
                EXPECT_EQ(props.height, 101);
                EXPECT_EQ(props.fps, fps);
                return true;
            }
            catch (ParseGreetingError&) {
                return false;
            }
        };

        // Any FPS is allowed for an image
        EXPECT_TRUE(parse(media::MediaType::image, 0));
        EXPECT_TRUE(parse(media::MediaType::image, 1));
        EXPECT_TRUE(parse(media::MediaType::image, 10));
        EXPECT_TRUE(parse(media::MediaType::image, 30));
        EXPECT_TRUE(parse(media::MediaType::image, 31));
        EXPECT_TRUE(parse(media::MediaType::image, 255));


        EXPECT_FALSE(parse(media::MediaType::video, 0));
        EXPECT_TRUE(parse(media::MediaType::video, 1));
        EXPECT_TRUE(parse(media::MediaType::video, 10));
        EXPECT_TRUE(parse(media::MediaType::video, 30));
        EXPECT_FALSE(parse(media::MediaType::video, 31));  // Max fps enforced for videos
        EXPECT_FALSE(parse(media::MediaType::video, 255)); // Max fps enforced for videos
    }

    TEST_F(CommunicationTest, ParseClientGreetingTooFewBytes) {
        try {
            auto props = ParseClientGreeting({{{1920, 1080}}, 60}, {1});
            FAIL();
        }
        catch (ParseGreetingError&) {
        }
    }

    TEST_F(CommunicationTest, ParseClientGreetingTooManyBytes) {
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