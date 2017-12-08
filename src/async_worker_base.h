#ifndef SRC_ASYNC_WORKER_BASE_H
#define SRC_ASYNC_WORKER_BASE_H

#include "common.h"
#include <uv.h>

template <typename T, typename U>
class AsyncWorkerBase {
  public:
    AsyncWorkerBase(uv_loop_t* loop, U& input);

    int work();

    // called on worker thread
    virtual T* onwork(U& input) = 0;

    // called on loop thread
    virtual void ondone(T* result, int status) = 0;

  private:
    typedef struct {
      T* result;
      U* input;
      uv_work_t req;
      AsyncWorkerBase* self;
    } work_t;

    static void work_cb_(uv_work_t* req);
    static void after_work_cb_(uv_work_t* req, int status);

    uv_loop_t* loop_;
    work_t work_;
};

#endif
