#include "common.h"
#include <uv.h>

static int work_cb_count;
static int after_work_cb_count;
static uv_work_t work_req;
static char data;

static void work_cb(uv_work_t* req) {
  ASSERT(req == &work_req);
  ASSERT(req->data == &data);
  work_cb_count++;
}


static void after_work_cb(uv_work_t* req, int status) {
  ASSERT(status == 0);
  ASSERT(req == &work_req);
  ASSERT(req->data == &data);
  after_work_cb_count++;
}

int main(int argc, char *argv[]) {
  int r;

  work_req.data = &data;
  r = uv_queue_work(uv_default_loop(), &work_req, work_cb, after_work_cb);
  ASSERT(r == 0);
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  ASSERT(work_cb_count == 1);
  ASSERT(after_work_cb_count == 1);

  return 0;
}
