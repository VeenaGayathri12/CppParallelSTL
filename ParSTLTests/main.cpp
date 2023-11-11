// ParSTLTests.cpp : Defines the entry point for the console application.

#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <numeric>
#include <vector>

template <typename TFunc>
void RunAndMeasure(const char* title, TFunc func) {
  const auto start = std::chrono::steady_clock::now();
  auto ret = func();
  const auto end = std::chrono::steady_clock::now();
  std::cout << title << ": "
            << std::chrono::duration<double, std::milli>(end - start).count()
            << " ms, res " << ret << "\n";
}

int main() {
  std::vector<double> v(6000000, 0.5);

  RunAndMeasure("std::warm up", [&v] {
    return std::reduce(std::execution::seq, v.begin(), v.end(), 0.0);
  });

  RunAndMeasure("std::accumulate",
                [&v] { return std::accumulate(v.begin(), v.end(), 0.0); });

  RunAndMeasure("std::reduce, seq", [&v] {
    return std::reduce(std::execution::seq, v.begin(), v.end(), 0.0);
  });

  RunAndMeasure("std::reduce, par", [&v] {
    return std::reduce(std::execution::par, v.begin(), v.end(), 0.0);
  });

  RunAndMeasure("std::reduce, par_unseq", [&v] {
    return std::reduce(std::execution::par_unseq, v.begin(), v.end(), 0.0);
  });

  RunAndMeasure("std::find, seq", [&v] {
    auto res = std::find(std::execution::seq, std::begin(v), std::end(v), 0.6);
    return res == std::end(v) ? 0.0 : 1.0;
  });

  RunAndMeasure("std::find, par", [&v] {
    auto res = std::find(std::execution::par, std::begin(v), std::end(v), 0.6);
    return res == std::end(v) ? 0.0 : 1.0;
  });

  return 0;
}
