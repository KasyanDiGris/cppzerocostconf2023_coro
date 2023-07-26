#pragma once

#include <coroutine>
#include <thread>

class SwitchThread {
public:
    SwitchThread(std::jthread& thread)
        : Thread_{thread}
    {}

    constexpr bool await_ready() {
        return false;
    }

    void await_suspend(std::coroutine_handle<> h) {
        if (Thread_.joinable()) {
            throw std::runtime_error{"Attempt to switch on same thread twice"};
        }

        Thread_ = std::jthread(
            [h]() {
                h.resume();
            }
        );
    }

    void await_resume() {}
private:
    std::jthread& Thread_;
};
