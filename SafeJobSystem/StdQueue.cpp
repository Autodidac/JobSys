#include "StdQueue.h"

StdQueueManager::StdQueueManager(size_t numThreads) : m_running(true) {
    for (size_t i = 0; i < numThreads; ++i) {
        m_threads.emplace_back(&StdQueueManager::workerThread, this);
    }
}

StdQueueManager::~StdQueueManager() {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_running = false;
    }
    m_condition.notify_all();
    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void StdQueueManager::addTask(const std::function<void()>& task) {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_taskQueue.push(task);
    }
    m_condition.notify_one();
}

void StdQueueManager::workerThread() {
    while (m_running || !m_taskQueue.empty()) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_condition.wait(lock, [this] {
                return !m_running || !m_taskQueue.empty();
                });
            if (!m_running && m_taskQueue.empty()) return;
            task = std::move(m_taskQueue.front());
            m_taskQueue.pop();
        }
        if (task) {
            task();
        }
    }
}
