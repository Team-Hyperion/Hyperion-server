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
        ConnectionBase c;

        c.mediaProp.SetType(media::MediaType::video);

        c.BeginOutFiles("Connection/save/media");
        c.OpenOutFile() << "h3llo\n"; // Fails assert if file was not closed after open

        const auto save_path = std::string("Connection/save/media/") + std::to_string(c.id) + ".mp4";
        EXPECT_TRUE(std::ifstream(save_path).is_open());

        // Cleanup
        c.FinishOutFile();
        std::filesystem::remove_all("Connection");
    }

    TEST(Connection, OpenOutFileNoDirectory) {
        ConnectionBase c;

        c.mediaProp.SetType(media::MediaType::video);

        c.BeginOutFiles("");

        const auto save_path = std::to_string(c.id) + ".mp4";
        EXPECT_TRUE(std::ifstream(save_path).is_open());


        // Cleanup
        c.FinishOutFile();
        std::filesystem::remove(save_path);
    }

    TEST(Connection, OpenOutFileAppend) {
        ConnectionBase c;

        c.mediaProp.SetType(media::MediaType::video);

        c.BeginOutFiles("");
        auto& a = c.OpenOutFile();
        a << "a";
        a.close();

        auto& b = c.OpenOutFile();
        b << "b\n";
        b.close();


        const auto save_path = std::to_string(c.id) + ".mp4";
        auto ifs             = std::ifstream(save_path);
        EXPECT_TRUE(ifs.is_open());
        std::string s;
        ifs >> s;
        EXPECT_EQ(s, "ab");


        // Cleanup
        ifs.close();
        c.FinishOutFile();
        std::filesystem::remove(save_path);
    }

    TEST(Connection, OpenOutFileMakeNewFile) {
        ConnectionBase c;

        c.mediaProp.SetType(media::MediaType::video);

        c.BeginOutFiles("Connection/save/media"); // Creates a file
        c.FinishOutFile();

        std::filesystem::remove_all("Connection");

        // Make directories + file
        c.OpenOutFile() << "h3llo\n";


        const auto save_path = std::string("Connection/save/media/") + std::to_string(c.id) + ".mp4";
        EXPECT_TRUE(std::ifstream(save_path).is_open());


        // Cleanup
        c.FinishOutFile();
        std::filesystem::remove_all("Connection");
    }
} // namespace hyperion::net