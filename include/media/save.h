// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#ifndef HYPERION_INCLUDE_MEDIA_SAVE_H
#define HYPERION_INCLUDE_MEDIA_SAVE_H
#pragma once

#include <fstream>
#include <stdexcept>
#include <string>

namespace hyperion::media
{
    struct MediaConfig;

    // Functions for saving media to path


    class ReadFileError final : public std::runtime_error
    {
        using runtime_error::runtime_error;
    };

    ///
    /// Creates save file and any directories towards it
    /// \exception std::filesystem::filesystem_error Failed to make directories
    /// \exception ReadFileError Failed to open ofstream
    [[nodiscard]] std::ofstream MakeSaveFile(const std::string& save_path);

} // namespace hyperion::media

#endif // HYPERION_INCLUDE_MEDIA_SAVE_H
