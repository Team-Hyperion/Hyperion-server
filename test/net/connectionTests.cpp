// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "net/connection.h"

#include <filesystem>

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

    TEST(Connection, OpenOutFileMakeFile) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.OpenOutFile("Connection/save/media");

            EXPECT_TRUE(std::ifstream("Connection/save/media/" + std::to_string(c.id) + "-1.mp4").is_open());
        }

        std::filesystem::remove_all("Connection");
    }

    TEST(Connection, OpenOutFileIncrementPart) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.OpenOutFile("Connection/save/media");
            c.OpenOutFile("Connection/save/media");
            c.OpenOutFile("Connection/save/media");
            c.OpenOutFile("Connection/save/media"); // Part 4

            EXPECT_TRUE(std::ifstream("Connection/save/media/" + std::to_string(c.id) + "-4.mp4").is_open());
        }

        std::filesystem::remove_all("Connection");
    }

    TEST(Connection, OpenOutFileNoDirectory) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.OpenOutFile("");

            EXPECT_TRUE(std::ifstream(std::to_string(c.id) + "-1.mp4").is_open());
        }

        std::filesystem::remove_all("Connection");
    }
} // namespace hyperion::net