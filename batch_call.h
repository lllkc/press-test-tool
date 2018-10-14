#pragma once
#include "task_manager.h"

namespace ll {
namespace utils {
template <typename DoWorkOnce>
int BatchCall(DoWorkOnce &&fn, uint32_t qps, uint32_t threadCnt,
              uint32_t *failedCnt) {
  SmoothBursty rateLimiter(1);
  rateLimiter.SetRate(qps);

  uint32_t failedCntArray[threadCnt];
  uint32_t totalCntArray[threadCnt];
  std::vector<std::thread> vecThreads;
  vecThreads.reserve(threadCnt);

  timeval beginTime;
  gettimeofday(&beginTime, nullptr);

  for (uint32_t i = 0; i < threadCnt; ++i) {
    auto failedCntPtr = &failedCntArray[i];
    auto totalCntPtr = &totalCntArray[i];

    auto taskId = i;
    auto workingFun = [&fn, &rateLimiter, failedCntPtr, totalCntPtr, taskId]() {
      TaskManager<DoWorkOnce> taskManager(std::forward<DoWorkOnce>(fn),
                                          rateLimiter, taskId);
      taskManager.Run();
      *failedCntPtr = taskManager.GetFailerCnt();
      *totalCntPtr = taskManager.GetTotalCnt();
    };
    vecThreads.push_back(std::thread(workingFun));
  }

  *failedCnt = 0;
  uint32_t totalCnt = 0;
  for (uint32_t i = 0; i < threadCnt; ++i) {
    vecThreads[i].join();
    *failedCnt += failedCntArray[i];
    totalCnt += totalCntArray[i];
  }

  timeval endTime;
  gettimeofday(&endTime, nullptr);
  uint32_t costMs = ((endTime.tv_sec - beginTime.tv_sec) * 1e6 +
                     endTime.tv_usec - beginTime.tv_usec) /
                    1e3;

  printf(
      "All Thread end. Cost %u ms. Process count: %u, Failed count: %u, "
      "Success rate: %.2f%%, Avg cost: %.2f ms, Qps: %.2f\n",
      costMs, totalCnt, *failedCnt,
      ((totalCnt > 0) ? (100.0 * (totalCnt - *failedCnt) / totalCnt) : 100.0),
      ((totalCnt > 0) ? (1.0 * costMs / totalCnt) : 0.0),
      ((totalCnt > 0) ? (totalCnt * 1000.0 / costMs) : 0.0));
  return 0;
}
};
};
