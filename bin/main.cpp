#include <lib/TTaskScheduler.h>

int main() {
    int a = 1;
    int b = 10;
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](int a) { return 100 * a; }, a);

    auto id2 = scheduler.add([](int b) { return b * 7; }, scheduler.getFutureResult<int>(id1));

    auto id3 = scheduler.add([](int b) { return -b; }, scheduler.getFutureResult<int>(id2));


    scheduler.executeAll();
    std::cout << scheduler.getFutureResult<int>(id3) << ' ';
}