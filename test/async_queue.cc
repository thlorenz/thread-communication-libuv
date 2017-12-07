#include "common.h"
#include <uv.h>

typedef struct {
  int cb_count = 0;
  int after_cb_count = 0;
  uv_work_t req;
} work_t;

static void work_cb(uv_work_t* req) {
  work_t* work = static_cast<work_t*>(req->data);
  work->cb_count++;
}

static void after_work_cb(uv_work_t* req, int status) {
  work_t* work = static_cast<work_t*>(req->data);
  ASSERT(status == 0);
  work->after_cb_count++;
}

int main(int argc, char *argv[]) {
  int r;

  uv_loop_t* loop = uv_default_loop();
  work_t work;
  work.req.data = &work;

  r = uv_queue_work(loop, &work.req, work_cb, after_work_cb);
  ASSERT(r == 0);

  uv_run(loop, UV_RUN_DEFAULT);

  ASSERT(work.cb_count == 1);
  ASSERT(work.after_cb_count == 1);

  return 0;
}
