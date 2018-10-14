#pragma once
#include "rate_limiter.h"
#include <functional>
#include <stdio.h>
#include <sys/time.h>
#include <thread>
#include <utility>
#include <vector>

namespace ll {
namespace utils {
template <typename Fn> class TaskManager {
public:
  TaskManager(Fn fn, SmoothBursty &rateLimiter, uint32_t taskId)
      : m_doWorkFn(std::forward<Fn>(fn)), m_taskId(taskId),
        m_rateLimiter(rateLimiter), m_doneCnt(0), m_failedCnt(0),
        m_successCnt(0), m_shouldStop(false), m_totalCostTime(0) {
    gettimeofday(&m_beginTime, nullptr);
  }

  ~TaskManager() {
    timeval endTime;
    gettimeofday(&endTime, nullptr);
    uint32_t costTime = ((endTime.tv_sec - m_beginTime.tv_sec) * 1e6 +
                         endTime.tv_usec - m_beginTime.tv_usec) /
                        1e3;
    printf("Task %u end. Process count: %u, Failed count: %u, "
           "Success rate: %.2f%%, Avg cost: %.2f ms, Qps: %.2f\n",
           m_taskId, m_doneCnt, m_failedCnt,
           ((m_doneCnt > 0) ? (100.0 * m_successCnt / m_doneCnt) : 100),
           ((m_doneCnt > 0) ? (1.0 * m_totalCostTime / m_doneCnt) : 0.0),
           ((m_doneCnt > 0) ? (m_doneCnt * 1000.0 / costTime) : 0.0));
  }
  void Run() {
    int ret = 0;
    timeval beginTime;
    timeval endTime;
    while (!m_shouldStop) {
      m_rateLimiter.Aquire();

      gettimeofday(&beginTime, nullptr);
      ret = m_doWorkFn(m_shouldStop);
      gettimeofday(&endTime, nullptr);
      if (!m_shouldStop) {
        ++m_doneCnt;
        m_totalCostTime += ((endTime.tv_sec - beginTime.tv_sec) * 1e6 +
                            endTime.tv_usec - beginTime.tv_usec) /
                           1e3;
        if (0 != ret)
          ++m_failedCnt;
        else
          ++m_successCnt;
      }
    }
  }
  uint32_t GetTotalCnt() { return m_doneCnt; }
  uint32_t GetFailerCnt() { return m_failedCnt; }

private:
  Fn m_doWorkFn;
  uint32_t m_taskId;
  SmoothBursty &m_rateLimiter;
  uint32_t m_doneCnt;
  uint32_t m_failedCnt;
  uint32_t m_successCnt;
  bool m_shouldStop;
  timeval m_beginTime;
  uint32_t m_totalCostTime;
};
};
};
