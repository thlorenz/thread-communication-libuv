#include "common.h"
#include <uv.h>

static uv_loop_t* loop;
int main(int argc, char *argv[]) {
  int r;
  loop = uv_default_loop();
  r = uv_run(loop, UV_RUN_DEFAULT);
  ASSERT(r == 0);
  return 0;
}
