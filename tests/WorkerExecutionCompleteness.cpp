#include <gtest/gtest.h>
#include <atomic>
#include "../src/Worker.hpp"
#include <iostream>

class WorkerTest : public ::testing::TestWithParam<int> {
protected:
    void RunTestWithThreads(int thread_count) {
        std::atomic<int> counter{0};

        Worker worker(thread_count);

        int tasks = 1000;
        for (int i = 0; i < tasks; i++) {
            worker.push([&counter, i]() { counter.fetch_add(1, std::memory_order_relaxed); std::cout << "Task executed" << i << "\n"; });
        }


        worker.execute();

        worker.join();

        EXPECT_EQ(counter.load(), tasks);
    }
};

TEST_P(WorkerTest, AllTasksExecuted) {
    RunTestWithThreads(GetParam());
}

// Run with multiple thread counts
INSTANTIATE_TEST_SUITE_P(
    ThreadCounts,
    WorkerTest,
    ::testing::Values(1)
);
