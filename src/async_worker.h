#ifndef SRC_ASYNC_WORKER_BASE_H
#define SRC_ASYNC_WORKER_BASE_H

#include "common.h"
#include <uv.h>

template <typename T, typename U>
class AsyncWorker {
  public:
    AsyncWorker(uv_loop_t* loop, U& input)
      : loop_(loop) {
      work_.input = &input;
      work_.req.data = &work_;
      work_.self = this;
    }

    int inline work() {
      return uv_queue_work(loop_, &this->work_.req, &work_cb_, &after_work_cb_);
    }

    // called on worker thread
    virtual T* onwork(U& input) = 0;

    // called on loop thread
    virtual void ondone(T* result, int status) = 0;

  private:
    typedef struct {
      T* result;
      U* input;
      uv_work_t req;
      AsyncWorker* self;
    } work_t;

    static void inline work_cb_(uv_work_t* req) {
      work_t* work = static_cast<work_t*>(req->data);
      work->result = work->self->onwork(*work->input);
    }

    static void inline after_work_cb_(uv_work_t* req, int status) {
      work_t* work = static_cast<work_t*>(req->data);
      work->self->ondone(work->result, status);
    }

    uv_loop_t* loop_;
    work_t work_;
};

#endif
