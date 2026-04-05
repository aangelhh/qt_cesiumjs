
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <vector>
#include <queue>
#include <functional>

namespace SDL_RTI {
class ThreadPool {
    public:
        ThreadPool(size_t);
        void enqueue(std::function<void()>);
        ~ThreadPool();

    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;

        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    };
}

#endif // THREADPOOL_H