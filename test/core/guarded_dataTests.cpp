// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include <gtest/gtest.h>

#include "core/guarded_data.h"

namespace hyperion::core
{
    TEST(GuardedData, GuardedAccess) {
        GuardedData<std::pair<int, float>> gd;

        GuardedAccess access(gd); // Mutex locked
        access->first = 12;
        EXPECT_EQ(gd.GetUnsafe().first, 12);

        auto& ref = *access;
        ref.first = 20;
        EXPECT_EQ(ref.first, 20);

        access.Get()->GetUnsafe().first = 9;
        EXPECT_EQ(gd.GetUnsafe().first, 9);
    }

    TEST(GuardedData, ConstGuardedAccess) {
        GuardedData<std::pair<int, float>> gd;

        const GuardedAccess access(gd);
        EXPECT_EQ(access->first, 0);
        EXPECT_EQ((*access).first, 0);

        EXPECT_EQ(access.Get(), &gd);
    }

    TEST(GuardedData, Get) {
        GuardedData<int> gd;

        auto& x = gd.GetUnsafe();
        x       = 54;

        EXPECT_EQ(gd.GetUnsafe(), 54);
    }

    TEST(GuardedData, ConstGet) {
        const GuardedData<int> gd;

        EXPECT_EQ(gd.GetUnsafe(), 0);
    }

} // namespace hyperion::core
