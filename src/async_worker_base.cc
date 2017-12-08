#include "async_worker_base.h"

template <typename T, typename U>
AsyncWorkerBase<T, U>::AsyncWorkerBase(uv_loop_t* loop, U& input)
  : loop_(loop) {
  work_.input = &input;
  work_.req.data = &work_;
  work_.self = this;
}

template <typename T, typename U>
int AsyncWorkerBase<T, U>::work() {
  return uv_queue_work(loop_, &this->work_.req, &work_cb_, &after_work_cb_);
}

template <typename T, typename U>
void AsyncWorkerBase<T, U>::work_cb_(uv_work_t* req) {
  work_t* work = static_cast<work_t*>(req->data);
  work->result = work->self->onwork(*work->input);
}

template <typename T, typename U>
void AsyncWorkerBase<T, U>::after_work_cb_(uv_work_t* req, int status) {
  work_t* work = static_cast<work_t*>(req->data);
  work->self->ondone(work->result, status);
}
