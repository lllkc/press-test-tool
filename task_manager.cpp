#include "task_manager.h"

namespace ll {
namespace utils {
template <typename Fn> void TaskManager<Fn>::Run() {
  int ret = 0;
  while (!m_shouldStop) {
    m_rateLimiter.Aquire();
    ret = m_doWorkFn(m_shouldStop);
    if (!m_shouldStop)
      ++m_doneCnt;
    if (0 != ret)
      ++m_failedCnt;
    else
      ++m_successCnt;
  }
}
};
};
