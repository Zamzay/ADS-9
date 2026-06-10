// Copyright 2022 NNTU-CS
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <locale>
#include <random>
#include <vector>
#include "tree.h"

int main() {
  const int MAX_N = 8;
const int K = 10;

std::random_device rd;
std::mt19937 gen(rd());

std::cout << "n\tcollectAll\tgetByOrder\tgetByRank\n";

for (int n = 1; n <= MAX_N; ++n) {
  std::vector<char> src;
  for (int i = 0; i < n; ++i) src.push_back('1' + i);

  PMTree tree(src);
  int64_t total = tree.getTotalPerms();
  if (total == 0) continue;

  auto start = std::chrono::steady_clock::now();
  auto all = collectAll(tree);
  auto end = std::chrono::steady_clock::now();
  int64_t time_all = std::chrono::duration_cast<
      std::chrono::microseconds>(end - start).count();

  std::vector<int> rand_vals(K);
  std::uniform_int_distribution<int> dist(1, total);
  for (int i = 0; i < K; ++i) rand_vals[i] = dist(gen);

  int64_t sum1 = 0;
  for (int idx : rand_vals) {
    start = std::chrono::steady_clock::now();
    auto perm = getByOrder(tree, idx);
    end = std::chrono::steady_clock::now();
    sum1 += std::chrono::duration_cast<
        std::chrono::microseconds>(end - start).count();
  }
  double avg1 = static_cast<double>(sum1) / K;

  int64_t sum2 = 0;
  for (int idx : rand_vals) {
    start = std::chrono::steady_clock::now();
    auto perm = getByRank(tree, idx);
    end = std::chrono::steady_clock::now();
    sum2 += std::chrono::duration_cast<
        std::chrono::microseconds>(end - start).count();
  }
  double avg2 = static_cast<double>(sum2) / K;

  std::cout << n << "\t" << time_all << "\t\t"
            << std::fixed << std::setprecision(2) << avg1 << "\t\t\t"
            << avg2 << "\n";
}
return 0;
}
