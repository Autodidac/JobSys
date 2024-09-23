#pragma once

#include <condition_variable>
#include <functional>
#include <queue>

class StdQueueManager {
public:
    explicit StdQueueManager(size_t numThreads);
    ~StdQueueManager();

    void addTask(const std::function<void()>& task);

private:
    void workerThread();

    std::queue<std::function<void()>> m_taskQueue;
    std::vector<std::thread> m_threads;
    std::atomic<bool> m_running;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
};
