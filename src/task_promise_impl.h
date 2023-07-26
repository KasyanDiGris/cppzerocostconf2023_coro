#pragma once

#if !defined(PROMISE_IMPL_INL_H)
#error "Do not include directly"

// For autocompletion
#include "task_promise.h"
#include "task.h"
#include <exception>
#endif

template<typename T>
void BaseTaskPromise<T>::ResumeWaitingCoro() {
    if (Task_->Handler_) {
        Task_->Handler_.resume();
    }
}

template<typename T>
void BaseTaskPromise<T>::unhandled_exception() {
    Task_->Exception_ = std::current_exception();
    ResumeWaitingCoro();
}

template<typename T>
Task<T> TaskPromise<T>::get_return_object() {
    return Task<T>{this};
}

template<typename T>
void TaskPromise<T>::return_value(auto&& arg) {
    BaseTaskPromise<T>::Task_->Result_.emplace(
        std::forward<decltype(arg)>(arg)
    );
    BaseTaskPromise<T>::ResumeWaitingCoro();
}

inline Task<void> TaskPromise<void>::get_return_object() {
    return Task<void>{this};
}

inline void TaskPromise<void>::return_void() {
    Task_->Completed_ = true;
    ResumeWaitingCoro();
}
