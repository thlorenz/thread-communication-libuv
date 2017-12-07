#ifndef SRC_ASYNC_WORKER_BASE_H
#define SRC_ASYNC_WORKER_BASE_H

#include "common.h"
#include <uv.h>

template <typename T, typename U>
class AsyncWorkerBase {
  public:
    AsyncWorkerBase(uv_loop_t& loop, U& input);

    // called on worker thread
    virtual T* onwork(U& input) = 0;

    // called on loop thread
    virtual void ondone(T* result) = 0;

    typedef struct {
      T* result;
      U& input;
      uv_work_t req;
    } work_t;

  private:
    void work_cb_(uv_work_t* req);
    void after_work_cb_(uv_work_t* req, int status);

    uv_loop_t& loop_;
    work_t work_;
};

#endif
