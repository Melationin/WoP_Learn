//
// Created by zhdds on 2026/6/2.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_THREAD_POOL_H
#define USTC_CG_2025_FRAMEWORK2D_THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

namespace USTC_CG {

class ThreadPool {
public:
    // 1. 创建线程池
    explicit ThreadPool(size_t n_threads = std::thread::hardware_concurrency()):n_threads_(n_threads) {
        for (size_t i = 0; i < n_threads; ++i) {
            workers_.emplace_back([this] {
                for (;;) {
                    std::function<void()> task;
                    {
                        std::unique_lock lock(mtx_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    // 2. 加入任务
    template<typename F>
    auto enqueue(F&& f) -> std::future<decltype(f())> {
        using R = decltype(f());
        auto p = std::make_shared<std::packaged_task<R()>>(std::forward<F>(f));
        auto future = p->get_future();
        {
            std::lock_guard lock(mtx_);
            tasks_.emplace([p] { (*p)(); });
        }
        cv_.notify_one();
        return future;
    }

    // 3. 等待所有任务结束
    void wait() {
        {
            std::unique_lock lock(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& w : workers_) {
            if (w.joinable()) w.join();
        }
    }

    size_t numThreads() const {
        return n_threads_;
    }

    ~ThreadPool() {
        if (!stop_) wait();
    }

private:
    size_t n_threads_;
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;
};

} // namespace USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_THREAD_POOL_H
