#pragma once

#include <coroutine>
#include <future>

template<typename T>
class BaseFutureCoroPromise  {
public:
    std::future<T> get_return_object() {
        return Promise_.get_future();
    }

    constexpr std::suspend_never initial_suspend() const noexcept {
        return {};
    }

    constexpr std::suspend_never final_suspend() const noexcept {
        return {};
    }

    void unhandled_exception() {
        Promise_.set_exception(std::current_exception());
    }

protected:
    std::promise<T> Promise_;
};

template<typename T, typename... TArgs>
class std::coroutine_traits<std::future<T>, TArgs...> {
public:
    class promise_type : public BaseFutureCoroPromise<T> {
    public:
        void return_value(auto&& arg) {
            BaseFutureCoroPromise<T>::Promise_.set_value(
                std::forward<decltype(arg)>(arg)
            );
        }
    };
};

template<typename... TArgs>
class std::coroutine_traits<std::future<void>, TArgs...> {
public:
    class promise_type : public BaseFutureCoroPromise<void> {
    public:
        void return_void() {
            Promise_.set_value();
        }
    };
};
