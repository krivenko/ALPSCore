/*
 * Copyright (C) 1998-2018 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */

/** @file concurrent_access.cpp
    Test concurrent construction of accumulators
*/
#include <alps/accumulators.hpp>
#include <gtest/gtest.h>

#include <future>

namespace aa=alps::accumulators;

static bool counting_thread() {
    aa::accumulator_set aset;
    aa::FullBinningAccumulator<double> named_acc("my_acc");
    aset << named_acc;
    auto acc=aset["my_acc"];
    acc << 1.0 << 2.0 << 3.0;
    return 3u==count(acc);
}

TEST(accumulatorsDeathTest, ConcurrentAccCtor) {
    ASSERT_EXIT({
            auto r1_future=std::async(std::launch::async,counting_thread);
            auto r2_future=std::async(std::launch::async,counting_thread);

            if (!r1_future.get()) {
                std::cerr << "Thread 1 failed";
                exit(1);
            }
            if (!r2_future.get()) {
                std::cerr << "Thread 2 failed";
                exit(2);
            }
            std::cerr << "Success";
            exit(0);
        }, ::testing::ExitedWithCode(0), "Success");
}
