#pragma once

#include <atomic>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>

namespace SafeJobSystem {

    enum class QueueType {
        StdQueue,
        WaitFreeQueue,
        LockFreeQueue
    };

    template <QueueType Type>
    class ThreadManager {
    public:
        ThreadManager(size_t numThreads);
        ~ThreadManager();
        void addTask(const std::function<void()>& task);

    private:
        void workerThread();
        void processTasks();

        std::queue<std::function<void()>> taskQueue;
        std::vector<std::thread> threads;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> stop;
    };

} // namespace SafeJobSystem
