#include <gtest/gtest.h>
//#include <gmock/gmock.h>
#include "lib/TTaskScheduler.h"
#include "cmath"

TEST(Tests, Equation) {
    float a = 1;
    float b = -2;
    float c = -15;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) { return -4 * a * c; }, a, c);

    auto id2 = scheduler.add([](float b, float v) { return b * b + v; }, b, scheduler.getFutureResult<float>(id1));

    auto id3 = scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));

    auto id4 = scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));

    auto id5 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));

    auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<float>(id5), 5);
    ASSERT_EQ(scheduler.getResult<float>(id6), -3);
}

TEST(Tests, Bool) {
    bool a = true;
    bool b = false;
    bool c = false;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](bool a, bool c) { return a == c; }, a, c);

    auto id2 = scheduler.add([](bool b, bool v) { return b == v; }, b, scheduler.getFutureResult<bool>(id1));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getFutureResult<bool>(id2), 1);
}

TEST(Tests, Int) {
    int a = 1;
    int b = 10;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a) { return 100 * a; }, a);

    auto id2 = scheduler.add([](int b) { return b * 7; }, scheduler.getFutureResult<int>(id1));

    auto id3 = scheduler.add([](int b) { return -b; }, scheduler.getFutureResult<int>(id2));


    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<int>(id3), -700);
}

TEST(Fourth, ZeroArg) {
    float a = 1;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a) { return -4 * a; }, a);

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<float>(id1), -4);
}
