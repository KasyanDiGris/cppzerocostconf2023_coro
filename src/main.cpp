#include "future_traits.h"
#include "task.h"
#include "switch_thread.h"

#include <iostream>

std::future<int> CoroReturnsFuture() {
    co_return 100;
}

std::future<void> CoroReturnsFutureException() {
    throw std::runtime_error("CoroReturnsFutureException failed");
    co_return;
}

Task<int> CoroReturnsTask() {
    co_return 200;
}

Task<void> NestedCoroWithForwardedStopToken() {
    auto stopToken = (co_await ThisCoro).StopToken;

    std::cout << "Stop token is forwarded: " << stopToken.stop_possible() << std::endl;
}

Task<void> CoroForwardsStopToken() {
    co_await NestedCoroWithForwardedStopToken();
}

Task<void> SwitchThreadCoro(std::jthread& targetThread) {
    std::cout << "Coro thread before switch " << std::this_thread::get_id() << std::endl;
    co_await SwitchThread(targetThread);
    // Prints same id until clang16
    // because of https://github.com/llvm/llvm-project/issues/47179
    std::cout << "Coro thread after switch " << std::this_thread::get_id() << std::endl;
}

int main() {
    std::cout << "CoroReturnsFuture result: " << CoroReturnsFuture().get() << std::endl;
    try {
        CoroReturnsFutureException().get();
    } catch(std::runtime_error e) {
        std::cout << e.what() << std::endl;
    }

    // task.AsFuture is the only way for sync wait for Task<T> result
    std::cout << "CoroReturnsTask result: " << CoroReturnsTask().AsFuture().get() << std::endl;

    std::stop_source stopSource;
    auto task = CoroForwardsStopToken();
    task.SetTopToken(stopSource.get_token());
    std::move(task).AsFuture().get();

    std::jthread thread;
    SwitchThreadCoro(thread).AsFuture().get();
}
