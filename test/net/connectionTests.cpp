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

    TEST(Connection, BeginOutFileMakeFile) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.BeginOutFiles("Connection/save/media");
            c.OpenOutFile() << "h3llo\n"; // Fails assert if file was not closed after open

            EXPECT_TRUE(std::ifstream("Connection/save/media/" + std::to_string(c.id) + "-1.mp4").is_open());
        }

        std::filesystem::remove_all("Connection");
    }

    TEST(Connection, OpenOutFileNoDirectory) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.BeginOutFiles("");

            EXPECT_TRUE(std::ifstream(std::to_string(c.id) + "-1.mp4").is_open());
        }

        std::filesystem::remove_all("Connection");
    }

    TEST(Connection, OpenOutFileAppend) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.BeginOutFiles("");
            auto& a = c.OpenOutFile();
            a << "a";
            a.close();

            auto& b = c.OpenOutFile();
            b << "b\n";
            b.close();


            auto ifs = std::ifstream(std::to_string(c.id) + "-1.mp4");
            EXPECT_TRUE(ifs.is_open());
            std::string s;
            ifs >> s;
            EXPECT_EQ(s, "ab");
        }

        std::filesystem::remove_all("Connection");
    }

    TEST(Connection, OpenOutFileMakeNewFile) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.BeginOutFiles("Connection/save/media");
            c.FinishOutFile();

            std::filesystem::remove_all("Connection");

            // Make directories + file
            c.OpenOutFile() << "h3llo\n";

            EXPECT_TRUE(std::ifstream("Connection/save/media/" + std::to_string(c.id) + "-2.mp4").is_open());
        }

        std::filesystem::remove_all("Connection");
    }

    TEST(Connection, FinishOutFileIncrementPart) {
        {
            ConnectionBase c;

            c.mediaProp.SetType(media::MediaType::video);

            c.BeginOutFiles("Connection/save/media");

            c.FinishOutFile();
            c.FinishOutFile();
            c.FinishOutFile();
            c.FinishOutFile();

            const auto path = std::string("Connection/save/media/") + std::to_string(c.id) + "-5.mp4";
            EXPECT_FALSE(std::ifstream(path).is_open());

            auto& of = c.OpenOutFile(); // Creates file
            EXPECT_TRUE(std::ifstream(path).is_open());
        }

        std::filesystem::remove_all("Connection");
    }
} // namespace hyperion::net