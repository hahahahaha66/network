// thread_pool.cpp

#include "../include/thread_pool.hpp"
#include <iostream>
#include <memory>

thread_pool::thread_pool() : stop(false) {
    for (int i = 0; i < MAX_NUM; i++) {
        threads.emplace_back(&thread_pool::work, this);
    }
}

thread_pool::~thread_pool() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        stop = true;
    }
    cond_work.notify_all();
    for (std::thread &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void thread_pool::work() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mutex);
            cond_work.wait(lock, [this] { return !workqueue.empty() || stop; });
            if (stop && workqueue.empty()) return;
            task = std::move(workqueue.front());
            workqueue.pop();
        }
        task();
    }
}

template<typename Func>
auto thread_pool::push_task(Func func) -> std::future<decltype(func())> {
    using ReturnType = decltype(func());

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::forward<Func>(func));
    std::future<ReturnType> result = task->get_future();
    {
        std::unique_lock<std::mutex> lock(mutex);
        workqueue.push([task]() { (*task)(); });
    }
    cond_work.notify_one();
    return result;
}
