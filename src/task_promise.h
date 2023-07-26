#pragma once

#include <coroutine>
#include <stop_token>
#include <utility>

template<typename T>
class Task;

class ThisCoroType {};
constexpr ThisCoroType ThisCoro;

struct ThisCoroParameters {
    std::stop_token StopToken;
};

template<typename T>
class BaseTaskPromise {
public:
    constexpr std::suspend_always initial_suspend() const noexcept {
        return {};
    }

    constexpr std::suspend_never final_suspend() const noexcept {
        return {};
    }

    void unhandled_exception();

    template<typename TT>
    Task<TT>& await_transform(Task<TT>& task) {
        if (!task.StopToken_.stop_possible()) {
            task.StopToken_ = Task_->StopToken_;
        }

        return task;
    }

    template<typename TT>
    Task<TT> await_transform(Task<TT>&& task) {
        if (!task.StopToken_.stop_possible()) {
            task.StopToken_ = Task_->StopToken_;
        }

        return std::move(task);
    }

    auto await_transform(const ThisCoroType&) {
        struct ThisCoroParameterAwaiter {
            constexpr bool await_ready() {
                return true;
            }

            void await_suspend(std::coroutine_handle<>) {}

            auto await_resume() {
                return ThisCoroParameters{
                    .StopToken = std::move(StopToken_)
                };
            }

            std::stop_token StopToken_;
        };

        return ThisCoroParameterAwaiter{
            .StopToken_ = Task_->StopToken_
        };
    }

    decltype(auto) await_transform(auto&& awaiter) {
        return std::forward<decltype(awaiter)>(awaiter);
    }

protected:
    friend class Task<T>;

    void ResumeWaitingCoro();
    Task<T>* Task_;
};

template<typename T>
class TaskPromise : public BaseTaskPromise<T> {
public:
    Task<T> get_return_object();
    void return_value(auto&& arg);
};

template<>
class TaskPromise<void> : public BaseTaskPromise<void> {
public:
    Task<void> get_return_object();
    void return_void();
};
