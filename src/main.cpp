#include "future_traits.h"
#include "task.h"
#include "switch_thread.h"

#include <iostream>

Task<int> Coro(std::jthread& thread) {
    std::cout << std::this_thread::get_id() << std::endl;
    co_await SwitchThread(thread);
    std::cout << std::this_thread::get_id() << std::endl;
    co_return 0;
}

int main() {
    std::jthread thread;
    Coro(thread).AsFuture().get();

    auto task = Coro(thread);
}
