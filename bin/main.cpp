#include <lib/TTaskScheduler.h>

int main() {
//    int a = 1;
//    int b = 10;
//    TTaskScheduler scheduler;
//
//    auto id1 = scheduler.add([](int a) { return 100 * a; }, a);
//
//    auto id2 = scheduler.add([](int b) { return b * 7; }, scheduler.getFutureResult<int>(id1));
//
//    auto id3 = scheduler.add([](int b) { return -b; }, scheduler.getFutureResult<int>(id2));
//
//
//    scheduler.executeAll();
//    std::cout << scheduler.getFutureResult<int>(id3) << ' ';
    bool a = true;
    bool b = false;
    bool c = false;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](bool a, bool c) { return a == c; }, a, c);

    auto id2 = scheduler.add([](bool b, bool v) { return b == v; }, b, scheduler.getFutureResult<bool>(id1));

    scheduler.executeAll();

//    ASSERT_EQ(scheduler.getFutureResult<bool>(id2), 1);
}