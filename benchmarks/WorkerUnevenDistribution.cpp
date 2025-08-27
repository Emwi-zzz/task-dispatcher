#include <benchmark/benchmark.h>
#include "Worker.hpp"
#include <random>
#include "../workloads/SyntheticTasks.hpp"

static std::vector<Task> fncs = workloads::makeSyntheticTasks();


static void WorkerUnevenDistribution(benchmark::State& state) {
    int thread_count = state.range(0);
    int tasks = 1000;

    for (auto _ : state) {
        std::atomic<int> counter{0};
        Worker worker(thread_count);

        Task t = fncs[0]; // compute some primes

        for(int i = 0; i < tasks; ++i) {
            worker.push([t]() {
                t();
            }, 0);
        }

        auto start = std::chrono::high_resolution_clock::now();
        worker.execute();
        worker.join();
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        benchmark::DoNotOptimize(duration);
        state.SetIterationTime(duration / 1e6);
    }
}

BENCHMARK(WorkerUnevenDistribution)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Iterations(10)->UseManualTime();

BENCHMARK_MAIN();
