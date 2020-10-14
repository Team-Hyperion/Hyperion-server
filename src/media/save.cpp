// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "media/save.h"

#include <filesystem>

using namespace hyperion;

std::ofstream media::MakeSaveFile(const std::string& save_path) {
    const auto path = std::filesystem::path(save_path);

    create_directories(path.parent_path());

    auto of = std::ofstream(save_path);
    if (!of.is_open()) {
        throw ReadFileError("Failed to open ofstream for save file");
    }

    return of;
}
