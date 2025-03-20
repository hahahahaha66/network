#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <future>
#include <queue>
#include <vector>
#include <pthread.h>
#include <iostream>
#include <memory>

const int MAX_NUM=12;

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
    std::vector<pthread_t> pthread_num;
    pthread_mutex_t mutex;
    pthread_cond_t cond_work;
};

#endif // THREAD_POOL_H