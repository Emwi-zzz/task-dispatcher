#include <gtest/gtest.h>
#include <vector>
#include <functional>
#include <random>
#include "Worker.hpp"

#include "../workloads/SyntheticTasks.hpp"

// Helper to run a Worker test with arbitrary threads and functions

void runWorkerFunctionTest(int threads, const std::vector<Task>& tasks) {
    Worker worker(threads);
    std::atomic<int> executed{0};

    std::mt19937 rng(42); // seed
    std::uniform_int_distribution<size_t> dist(0, tasks.size() - 1);

    const int n_pushes = 10000;

    for(int i = 0; i < n_pushes; ++i) {
        size_t idx = dist(rng);
        Task t = tasks[idx];

        worker.push([t]() {
            t();
        });
    }
    worker.execute();

    worker.join();

    int global_Abort_count = 0;
    int global_EMPTY_count = 0;
    int global_pop_count = 0;
    int global_steal_count = 0;

    for(int i = 0; i < threads; ++i) {
        int ABORT_count = worker.deques[i].ABORT_count->load(std::memory_order_relaxed);
        int EMPTY_count = worker.deques[i].EMPTY_count->load(std::memory_order_relaxed);
        int pop_count = worker.deques[i].pop_count->load(std::memory_order_relaxed);
        int steal_count = worker.deques[i].steal_count->load(std::memory_order_relaxed);
        std::cout << "Thread " << i << " - ABORT: " << ABORT_count << ", EMPTY: " << EMPTY_count 
                  << ", pop: " << pop_count << ", steal: " << steal_count 
                  << " successful steals: " << (steal_count - ABORT_count) << "\n";
        global_Abort_count += ABORT_count;
        global_EMPTY_count += EMPTY_count;
        global_pop_count += pop_count;
        global_steal_count += steal_count;
    }
    std::cout << "Global - ABORT: " << global_Abort_count << ", EMPTY: " << global_EMPTY_count 
              << ", pop: " << global_pop_count << ", steal: " << global_steal_count
              << " successful steals: " << (global_steal_count - global_Abort_count) << "\n";
    
    EXPECT_EQ(global_pop_count + global_steal_count - global_Abort_count - global_EMPTY_count, n_pushes);
    EXPECT_EQ(global_EMPTY_count, threads);
    #ifndef STEALING
        EXPECT_EQ(global_steal_count, 0);
    #endif
}


//TESTS


#ifdef TESTING

std::vector<Task> fncs = workloads::makeSyntheticTasks();

TEST(WorkerIntegration, RandomFunctionExecution_1Threads) {
    runWorkerFunctionTest(1, fncs);
}

TEST(WorkerIntegration, RandomFunctionExecution_2Threads) {
    runWorkerFunctionTest(2, fncs);
}

TEST(WorkerIntegration, RandomFunctionExecution_4Threads) {
    runWorkerFunctionTest(4, fncs);
}

TEST(WorkerIntegration, RandomFunctionExecution_8Threads) {
    runWorkerFunctionTest(8, fncs);
}

TEST(WorkerIntegration, RandomFunctionExecution_16Threads) {
    runWorkerFunctionTest(16, fncs);
}

#endif