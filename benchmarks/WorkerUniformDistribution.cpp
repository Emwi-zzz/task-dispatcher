#include <benchmark/benchmark.h>
#include "Worker.hpp"
#include <random>
#include "../workloads/SyntheticTasks.hpp"

static std::vector<Task> fncs = workloads::makeSyntheticTasks();


static void WorkerUniformDistribution(benchmark::State& state) {
    int thread_count = state.range(0);
    int tasks = 10000;

    for (auto _ : state) {
        std::atomic<int> counter{0};
        Worker worker(thread_count);

        std::mt19937 rng(42); // seed
        std::uniform_int_distribution<size_t> dist(0, fncs.size() - 1);

        for(int i = 0; i < tasks; ++i) {
            size_t idx = dist(rng);
            Task t = fncs[idx];

            worker.push([t]() {
                t();
            });
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

BENCHMARK(WorkerUniformDistribution)->Arg(1)->Arg(2)->Arg(4)->Arg(8)->Arg(12)->Arg(16)->Arg(20)
    ->Iterations(10)->UseManualTime();

BENCHMARK_MAIN();
