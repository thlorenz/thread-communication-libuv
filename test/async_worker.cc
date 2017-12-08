#include "async_worker_base.cc"
#include "common.h"
#include <uv.h>

uint64_t START_TIME = uv_hrtime() / 1E6;
#define CHUNK_SIZE 64000

typedef void (*done_cb_t)(int* result, int status);

class FileStreamAsyncWorker : public AsyncWorkerBase<int, const char> {
  public:
    FileStreamAsyncWorker(
        uv_loop_t* loop,
        const char& input,
        done_cb_t done_cb = noop
      ) : AsyncWorkerBase(loop, input), done_cb_(done_cb) {}

  private:
    int* onwork(const char& file) {
      log("onwork start");
      int* chunks = count_chunks(file, CHUNK_SIZE);
      log("onwork end");
      return chunks;
    }

    void ondone(int* result, int status) {
      log("ondone, calling final done");
      done_cb_(result, status);
    }

    static void noop(int* result, int status) {}
    done_cb_t done_cb_;
};

static void onfinalDone(int* result, int status) {
  ASSERT(status == 0);
  log("onfinalDone");
  fprintf(stderr, "read %d chunks\n", *result);
}

int main(int argc, char *argv[]) {
  uv_loop_t* loop = uv_default_loop();
  const char& file = *argv[0];

  // supplying onfinalDone is optional and demonstrates the ability to
  // handle the result inside the async worker classes or outside
  FileStreamAsyncWorker worker(loop, file, onfinalDone);
  int r = worker.work();
  ASSERT(r == 0);

  log("Starting loop");
  uv_run(loop, UV_RUN_DEFAULT);
}
