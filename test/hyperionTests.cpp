// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "core/crash_handler.h"
#include "core/filesystem.h"

int main(int ac, char* av[]) {
    hyperion::core::SetExecutingDirectory(av[0]);

    hyperion::core::RegisterCrashHandler();

    testing::InitGoogleTest(&ac, av);
    RUN_ALL_TESTS();
    return 0; // Always return 0 even if tests fail, as a non-zero return code is seen as abnormal test suite exit
}