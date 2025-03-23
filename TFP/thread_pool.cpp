#include "thread_pool.hpp"
#include <iostream>
#include <memory>

thread_pool::thread_pool() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_work, NULL);
    pthread_num.resize(MAX_NUM);
    stop = false;
    for (int i = 0; i < MAX_NUM; i++) {
        if (pthread_create(&pthread_num[i], NULL,
            [](void* arg) -> void* { static_cast<thread_pool*>(arg)->work(); return NULL; }, this) != 0) {
            std::cout << "thread create failed" << std::endl;
            exit(-1);
        }
    }
}

thread_pool::~thread_pool() {
    stop = true;
    pthread_cond_broadcast(&cond_work);
    for (int i = 0; i < MAX_NUM; i++) {
        pthread_join(pthread_num[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_work);
}

void thread_pool::work() {
    while (true) {
        pthread_mutex_lock(&mutex);
        std::function<void()> task;
        while (workqueue.empty() && !stop) {
            pthread_cond_wait(&cond_work, &mutex);
        }
        if (stop) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        task = std::move(workqueue.front());
        workqueue.pop();
        pthread_mutex_unlock(&mutex);
        task();
    }
    return;
}

template<typename Func>
auto thread_pool::push_task(Func func) -> std::future<decltype(func())> {
    using ReturnType = decltype(func());

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::forward<Func>(func));
    std::future<ReturnType> result = task->get_future();

    pthread_mutex_lock(&mutex);

    workqueue.push([task]() { (*task)(); });

    pthread_cond_signal(&cond_work);
    pthread_mutex_unlock(&mutex);
    return result;
}