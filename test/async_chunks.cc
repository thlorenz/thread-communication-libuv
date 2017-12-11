#include "common.h"
#include <uv.h>

uint64_t START_TIME = uv_hrtime() / 1E6;
#define CHUNK_SIZE 64000

typedef struct {
  std::istream& stream;
  size_t chunkSize;
  const char* chunk;
} loop_work_t;

static void onloopIteration(uv_idle_t* handle) {
  loop_work_t* loop_work = static_cast<loop_work_t*>(handle->data);

  std::vector<char> buffer(loop_work->chunkSize, 0);
  loop_work->stream.read(buffer.data(), buffer.size());
  loop_work->chunk = copy_buffer(buffer.data(), buffer.size());
  log("added chunk, waiting to process");

  if (loop_work->stream.eof()) {
    log("reached end of stream, stopping");
    uv_idle_stop(handle);
  }
}

int main(int argc, char *argv[]) {
  int r;
  uv_loop_t* loop = uv_default_loop();
  const char* file = argv[0];

  uv_idle_t idle;
  r = uv_idle_init(loop, &idle);
  ASSERT(r == 0);

  std::ifstream stream(file);
  loop_work_t loop_work = { .stream = stream, .chunkSize = CHUNK_SIZE };
  idle.data = (void*)(&loop_work);

  r = uv_idle_start(&idle, onloopIteration);
  ASSERT(r == 0);

  log("Starting loop");
  uv_run(loop, UV_RUN_DEFAULT);
}
