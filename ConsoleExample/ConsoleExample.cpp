#include "SafeJobSystem.h"
#include <chrono>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace SafeJobSystem;

std::ofstream logFile("benchmark_results.txt");
std::ostringstream logBuffer;

void log(const std::string& message, bool toFile = true) {
    if (toFile) {
        logFile << message << std::endl;
    }
    logBuffer << message << std::endl;
    std::cout << message << std::endl; // Print to console on a new line
}

void runBenchmark(const std::string& label, size_t numThreads, size_t numTasks) {
    log("Starting " + label + " with " + std::to_string(numThreads) + " threads and " + std::to_string(numTasks) + " tasks.");

    auto start = std::chrono::high_resolution_clock::now();

    size_t completedTasks = 0;
    size_t progressInterval = numTasks / 10; // Update progress every 10% of tasks

    if (label == "StdQueue") {
        ThreadManager<QueueType::StdQueue> manager(numThreads);
        std::vector<std::future<void>> futures;
        for (size_t i = 0; i < numTasks; ++i) {
            futures.emplace_back(std::async(std::launch::async, [&, numTasks, progressInterval]() {
                try {
                    manager.addTask([&, numTasks, progressInterval]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        completedTasks++;
                        if (completedTasks % progressInterval == 0) {
                            log("\rProgress: " + std::to_string(static_cast<double>(completedTasks) / numTasks * 100) + "%", false);
                        }
                        });
                }
                catch (const std::exception& e) {
                    log("Exception: " + std::string(e.what()), true);
                }
                }));
        }
        for (auto& future : futures) {
            future.get(); // Ensure all tasks complete
        }
    }
    else if (label == "WaitFreeQueue") {
        ThreadManager<QueueType::WaitFreeQueue> manager(numThreads);
        std::vector<std::future<void>> futures;
        for (size_t i = 0; i < numTasks; ++i) {
            futures.emplace_back(std::async(std::launch::async, [&, numTasks, progressInterval]() {
                try {
                    manager.addTask([&, numTasks, progressInterval]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        completedTasks++;
                        if (completedTasks % progressInterval == 0) {
                            log("\rProgress: " + std::to_string(static_cast<double>(completedTasks) / numTasks * 100) + "%", false);
                        }
                        });
                }
                catch (const std::exception& e) {
                    log("Exception: " + std::string(e.what()), true);
                }
                }));
        }
        for (auto& future : futures) {
            future.get(); // Ensure all tasks complete
        }
    }
    else if (label == "LockFreeQueue") {
        ThreadManager<QueueType::LockFreeQueue> manager(numThreads);
        std::vector<std::future<void>> futures;
        for (size_t i = 0; i < numTasks; ++i) {
            futures.emplace_back(std::async(std::launch::async, [&, numTasks, progressInterval]() {
                try {
                    manager.addTask([&, numTasks, progressInterval]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                        completedTasks++;
                        if (completedTasks % progressInterval == 0) {
                            log("\rProgress: " + std::to_string(static_cast<double>(completedTasks) / numTasks * 100) + "%", false);
                        }
                        });
                }
                catch (const std::exception& e) {
                    log("Exception: " + std::string(e.what()), true);
                }
                }));
        }
        for (auto& future : futures) {
            future.get(); // Ensure all tasks complete
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    log("\n" + label + " task processing took " + std::to_string(duration.count()) + " seconds.");
}

int main() {
    size_t numThreads = 6;
    size_t numTasks = 10000;

    runBenchmark("StdQueue", numThreads, numTasks);
    runBenchmark("WaitFreeQueue", numThreads, numTasks);
    runBenchmark("LockFreeQueue", numThreads, numTasks);

    logFile.close(); // Close the log file

    // Print the contents of the log file to the console
    std::ifstream inputFile("benchmark_results.txt");
    std::string line;
    while (std::getline(inputFile, line)) {
        std::cout << line << std::endl;
    }

    return 0;
}
