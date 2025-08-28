# TaskDispatcher

[![C++ Standard](https://img.shields.io/badge/C++-17-blue.svg)](#)
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](#)
[![Tests](https://img.shields.io/badge/tests-passing-brightgreen.svg)](#)
[![Benchmarks](https://img.shields.io/badge/benchmarks-available-orange.svg)](#)

High-performance multithreaded task dispatcher with work-stealing deques.  
Designed in modern **C++17** for efficient parallel task execution and load balancing across CPU cores.

---

## 📑 Contents
- [Overview](#overview)
- [Functionality](#functionality)
- [Building, Testing & Benchmarking](#building--testing--benchmarking)
- [Benchmark Results](#benchmark-results)

## Overview

**TaskDispatcher** is a high-performance, multithreaded task scheduler written in modern **C++17**.  
Originally designed as part of a game engine, this library demonstrates efficient task execution and load balancing, making it ideal for executing game logic or fixed-timestep updates across multiple threads.

The core of the library is the **WSDeque**, a work-stealing deque inspired by [Dynamic Circular Work-Stealing Deques](https://www.dre.vanderbilt.edu/~schmidt/PDF/work-stealing-dequeue.pdf) by David Chase and Yossi Lev. This design allows threads to efficiently steal tasks from one another when idle, ensuring balanced utilization across cores.

The library supports both **uniform and uneven workloads** (see [Benchmarks](#benchmark-results)), and is implemented as a small, header-friendly library for easy integration with minimal overhead.

This project serves both as a **skills showcase** for potential employers and as a practical tool for developers who need a flexible, high-performance task dispatcher.

## Functionality

**TaskDispatcher** exposes a clean interface for dispatching tasks (`Task`) across multiple threads.  

> **Note:** `Task` is defined as `std::function<void()>`, meaning any callable object with no arguments and no return value can be scheduled.

| Function | Description |
|----------|-------------|
| `push(Task&& task)` | Adds a task to the dispatcher. The dispatcher automatically assigns it to a thread. |
| `push(Task&& task, int thread_id)` | Adds a task to a specific thread's queue. Useful for controlling task locality. |
| `bool is_executing()` | Returns `true` if any thread is still processing tasks. |
| `execute()` | Starts executing all tasks in all threads. |
| `join()` | Waits for all threads to complete execution. |
| `set_log_size(int log_size)` | Sets the internal log size used by each worker thread (affects deque capacity). |

### Usage Example

```cpp
#include "Worker.hpp"

int main() {
    Worker worker(4); // Create a worker with 4 threads

    // Push tasks
    worker.push([](){ std::cout << "Task 1 executed\n"; });
    worker.push([](){ std::cout << "Task 2 executed\n"; });

    // Execute all tasks
    worker.execute();

    // Wait for all threads to finish
    worker.join();

    return 0;
}
```

### Tips & Notes

- `.join()` can be called multiple times. It safely waits even if some threads are not currently running, but cannot be used before first `.execute()`
- Make sure you've called `.join()`. before calling `.execute()` for second time on the same object
- For advanced task scheduling, you can use **two instances of Worker**:
  - One for **pushing tasks**.
  - One for **executing tasks**.
  - Swap them between ticks to maintain smooth task flow.


## Building, Testing & Benchmarking

### Build Options

- **Build tests** (GoogleTest + `TESTING` definitions):
```bash
cmake -B build -DBUILD_TESTING_MODE=ON
cmake --build build -j
ctest --output-on-failure
````

* **Build benchmarks** (Release mode):

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Benchmarks will be compiled to `build/benchmarks/`.

* **Build library only**:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARK=OFF
cmake --build build -j
```

After building, the static library and headers will be available at:

```
build/libTaskDispatcherLib.a
```

### Stealing Control

* Work-stealing can be **enabled or disabled** for all threads via the CMake option:

```bash
-DENABLE_STEALING=OFF
```

By default, work-stealing is enabled:

```bash
-DENABLE_STEALING=ON
```

## Benchmark Results

### Test Environment

| Feature | Specification |
|---------|---------------|
| CPU Architecture | x86_64 |
| CPU | 12th Gen Intel Core i7-1255U |
| Cores / Threads | 12 cores, 2 threads per core |
| Base Clock | 400 MHz |
| Max Clock | 4700 MHz |
| L1 Data Cache | 48 KiB (x6) |
| L1 Instruction Cache | 32 KiB (x6) |
| L2 Unified Cache | 1280 KiB (x6) |
| L3 Unified Cache | 12288 KiB (x1) |

---

### Running Benchmarks

```bash
./build/benchmarks/WorkerUniformDistribution --benchmark_time_unit=ms
./build/benchmarks/WorkerUnevenDistribution --benchmark_time_unit=ms
````

* **WorkerUniformDistribution**: tasks are evenly distributed across threads.
* **WorkerUnevenDistribution**: all tasks initially loaded into a single WSDeque, tests work-stealing.

---

### Results

**WorkerUniformDistribution**

| Threads | Time (ms) | CPU (ms) | Iterations |
| ------- | --------- | -------- | ---------- |
| 1       | 2121      | 0.750    | 10         |
| 2       | 1319      | 0.732    | 10         |
| 4       | 896       | 0.817    | 10         |
| 8       | 686       | 1.20     | 10         |
| 12      | 629       | 1.64     | 10         |
| 16      | 632       | 1.95     | 10         |
| 20      | 625       | 1.97     | 10         |

**WorkerUnevenDistribution**

| Threads | Time (ms) | CPU (ms) | Iterations |
| ------- | --------- | -------- | ---------- |
| 1       | 71.8      | 0.123    | 10         |
| 2       | 43.3      | 0.151    | 10         |
| 4       | 28.5      | 0.161    | 10         |
| 8       | 19.3      | 0.402    | 10         |
| 12      | 16.2      | 0.496    | 10         |
| 16      | 16.7      | 0.653    | 10         |
| 20      | 18.6      | 0.873    | 10         |

---

**WorkerUniformDistribution** (lower is better)

```
Threads
1  | ████████████████████████ 2121 ms
2  | ██████████████ 1319 ms
4  | █████████ 896 ms
8  | ██████ 686 ms
12 | █████ 629 ms
16 | █████ 632 ms
20 | █████ 625 ms
```

**WorkerUnevenDistribution** (lower is better)

```
Threads
1  | █████████████████████████████████████████████████████ 71.8 ms
2  | ████████████████████████████████ 43.3 ms
4  | ██████████████████ 28.5 ms
8  | ██████████ 19.3 ms
12 | ██████ 16.2 ms
16 | ██████ 16.7 ms
20 | ██████ 18.6 ms
```

---

### Observations

* **Scaling**: Both uniform and uneven workloads benefit from multiple threads. Significant speedup is observed up to 12 threads (physical cores).
* **Saturation**: Beyond 12 threads, adding more threads shows little to no improvement and may slightly increase runtime due to context switching.
* **Work Stealing**: Uneven workload demonstrates the effectiveness of work-stealing. Tasks are balanced dynamically across threads.
* **Efficiency**: Uniform workload shows near-linear speedup until hardware limits are reached. CPU utilization grows modestly above core count.

> ⚡ This demonstrates that `TaskDispatcher` is efficient for both uniform and uneven task distributions, with effective dynamic load balancing across cores.