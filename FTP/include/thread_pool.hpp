//thread_pool.hpp

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <future>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

const int MAX_NUM = 12;

class thread_pool {
public:
    thread_pool();
    ~thread_pool();

    template<typename Func>
    auto push_task(Func func) -> std::future<decltype(func())>;

private:
    void work();

    std::queue<std::function<void()>> workqueue;
    bool stop;
    std::vector<std::thread> threads;
    std::mutex mutex;
    std::condition_variable cond_work;
};

#endif // THREAD_POOL_H
