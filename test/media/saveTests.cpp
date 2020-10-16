// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "media/save.h"

#include <filesystem>

namespace hyperion::media
{
    class MediaSaveTest : public testing::Test
    {
    protected:
        void TearDown() override {
            std::filesystem::remove_all("MediaSave");
        }
    };

    TEST_F(MediaSaveTest, MakeSaveFile) {
        {
            auto ofstream = MakeSaveFile("MediaSave/MakeSaveFile/a/b/c/t.txt");

            ofstream << "Test"
                     << "\n";
        }

        // Must flush ofstream first
        {
            auto ifstream = std::ifstream("MediaSave/MakeSaveFile/a/b/c/t.txt");
            EXPECT_TRUE(ifstream.is_open());

            std::string s;
            ifstream >> s;

            EXPECT_EQ(s, "Test");
        }
    }

    TEST_F(MediaSaveTest, MakeSaveFileInvalidPath) {
        try {
            auto ofstream = MakeSaveFile("MediaSave/MakeSaveFile/a/b/c/");
            FAIL();
        }
        catch (ReadFileError&) {
        }
    }
} // namespace hyperion::media