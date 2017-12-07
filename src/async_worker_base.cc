#include "async_worker_base.h"

template <typename T, typename U>
AsyncWorkerBase<T, U>::AsyncWorkerBase(uv_loop_t& loop, U& input)
  : loop_(loop) {
  work_.input = input;
  work_.req.data = &work_;

  int r = uv_queue_work(loop, &work_.req, &onwork, &ondone);
  ASSERT(r == 0);
}

template <typename T, typename U>
void AsyncWorkerBase<T, U>::work_cb_(uv_work_t* req) {
  work_t* work = static_cast<work_t*>(req->data);
  work->result = onwork(work->input);
}

template <typename T, typename U>
void AsyncWorkerBase<T, U>::after_work_cb_(uv_work_t* req, int status) {
  work_t* work = static_cast<work_t*>(req->data);
  ASSERT(status == 0);
  ondone(work->result);
}
