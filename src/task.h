#pragma once

#include "future_traits.h"
#include "task_promise.h"

#include <coroutine>
#include <exception>
#include <optional>
#include <stop_token>

template<typename T>
class BaseTask {
public:
    using promise_type = TaskPromise<T>;

    BaseTask(BaseTask&& other) = default;

    BaseTask(promise_type* promise)
        : Promise_{promise}
    {
    }

    void Start() {
        if (!Started_) {
            Started_ = true;
            std::coroutine_handle<promise_type>::from_promise(*Promise_).resume();
        }
    }

    void await_suspend(auto h) {
        Handler_ = h;
    }

    void SetTopToken(std::stop_token token) {
        StopToken_ = std::move(token);
    }

protected:
    template<typename TT>
    friend class BaseTaskPromise;

    promise_type* Promise_;
    std::exception_ptr Exception_;
    std::coroutine_handle<> Handler_;
    bool Started_ = false;
    std::stop_token StopToken_;
};


template<typename T>
class Task : public BaseTask<T> {
public:
    using BaseTask<T>::BaseTask;

    Task(Task<T>&& other)
        : BaseTask<T>(static_cast<BaseTask<T>&&>(other))
    {
        BaseTask<T>::Promise_->Task_ = this;
        Result_ = std::move(other.Result_);
    }

    bool await_ready() {
        BaseTask<T>::Start();
        return (Result_ || BaseTask<T>::Exception_);
    }

    T await_resume() {
        if (BaseTask<T>::Exception_) {
            std::rethrow_exception(BaseTask<T>::Exception_);
        }

        return std::move(*Result_);
    }

    std::future<T> AsFuture() && {
        auto self = std::move(*this);
        co_return co_await self;
    }

private:
    friend class TaskPromise<T>;

    std::optional<T> Result_;
};


template<>
class Task<void> : public BaseTask<void> {
public:
    using BaseTask<void>::BaseTask;

    Task(Task<void>&& other)
        : BaseTask<void>(static_cast<BaseTask<void>&&>(other))
    {
        Promise_->Task_ = this;
        Completed_ = other.Completed_;
    }

    bool await_ready() {
        Start();
        return (Completed_ || Exception_);
    }

    void await_resume() {
        if (Exception_) {
            std::rethrow_exception(Exception_);
        }
    }

    std::future<void> AsFuture() && {
        auto self = std::move(*this);
        co_return co_await self;
    }

private:
    friend class TaskPromise<void>;
    bool Completed_ = false;
};


#define PROMISE_IMPL_INL_H
#include "task_promise_impl.h"
#undef PROMISE_IMPL_INL_H
