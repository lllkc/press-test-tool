#include "batch_call.h"
#include <atomic>
#include <iostream>
//#include <sys/time.h>
#include <unistd.h>

using namespace std;
int main() {
  using namespace ll::utils;

  std::atomic<int> counter(0);
  const int TOTAL_TASK_CNT = 30000;
  constexpr auto QPS = 3000;

  auto processFun = [&](bool &shouldStop) -> int {
    if (counter > TOTAL_TASK_CNT) {
      cout << "[1]current counter=" << counter << ", shouldStop!" << endl;
      shouldStop = true;
      return 0;
    }
    auto currentCnt = counter.fetch_add(1);
    if (currentCnt > TOTAL_TASK_CNT) {
      cout << "[2]current counter=" << currentCnt << ", shouldStop!" << endl;
      shouldStop = true;
      return 0;
    }
    // do work
    usleep(1000 * 20);

    return 0;
  };

  uint32_t failedCnt;
  int ret = BatchCall(processFun, QPS, 100, &failedCnt);
  if (0 != ret) {
    cout << "batchCall failed, ret=" << ret << endl;
  }
  return 0;
}
