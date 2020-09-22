// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "hyperion.h"

#include "core/crash_handler.h"
#include "core/filesystem.h"
#include "core/resource_guard.h"

int main(int argc, char* argv[]) {
    using namespace hyperion;

    core::SetExecutingDirectory(argv[0]);

    // Log file
    core::ResourceGuard log_guard(&core::CloseLogFile);
    core::OpenLogFile();

    core::RegisterCrashHandler();

    // Initial startup message
    LOG_MESSAGE_F(info, "%s | %s build, version: %s\n\n", HYPERION_BUILD_TARGET_PLATFORM, BUILD_TYPE, HYPERION_VERSION);

    // Initialize server

    LOG_MESSAGE(info, "goodbye!");
    return 0;
}
