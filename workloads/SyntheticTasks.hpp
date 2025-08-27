#pragma once
#include <vector>
#include <cmath>

using Task = std::function<void()>;


namespace workloads {

inline std::vector<Task> makeSyntheticTasks() {
    return {
        [](){ // compute some primes
            volatile int count = 0;
            for(int i = 2; i < 5000; ++i) {
                bool prime = true;
                for(int j = 2; j <= std::sqrt(i); ++j) {
                    if(i % j == 0) { prime = false; break; }
                }
                if(prime) count++;
            }
        },
        [](){ // sum of squares
            volatile long long sum = 0;
            for(int i = 0; i < 100000; ++i)
                sum += i*i;
        },
        [](){ // compute sine/cosine table
            volatile double result = 0;
            for(int i = 0; i < 100000; ++i)
                result += std::sin(i)*std::cos(i);
        },
        [](){ // small matrix multiplication
            volatile double mat[50][50] = {};
            for(int i = 0; i < 50; ++i)
                for(int j = 0; j < 50; ++j)
                    for(int k = 0; k < 50; ++k)
                        mat[i][j] += i*k*j*0.0001;
        },
        [](){ // simple Fibonacci iteration
            volatile int a = 0, b = 1;
            for(int i = 0; i < 30; ++i) {
                int tmp = a + b;
                a = b;
                b = tmp;
            }
        }
    };
}

}
