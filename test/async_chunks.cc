#include "async_worker_base.cc"
#include "common.h"
#include <uv.h>

uint64_t START_TIME = uv_hrtime() / 1E6;
#define CHUNK_SIZE 64000

typedef struct {
  std::istream& stream;
  size_t chunkSize;
  const char* chunk;
  bool chunkToProcess;
  bool done;
  uv_mutex_t mutex;
} loop_work_t;

class ChunkProcessor : public AsyncWorkerBase<int, loop_work_t> {
  public:
    ChunkProcessor(
        uv_loop_t* loop,
        loop_work_t& work
      ) : AsyncWorkerBase(loop, work) {}

  private:
    int* onwork(loop_work_t& work) {
      log("onwork start work");
      int* chunks = new int(0);
      bool done;
      bool chunkToProcess;
      do {
        uv_mutex_lock(&work.mutex);
        {
          log("processor {");
          done = work.done;
          chunkToProcess = work.chunkToProcess;
          work.chunkToProcess = false;
          if (chunkToProcess) { 
            log("  will process new chunk");
          } else {
            log("  no chunk to process");
          }
        }
        uv_mutex_unlock(&work.mutex);
        log("}");
        if (chunkToProcess) (*chunks)++;
        // pretend we need time to process this
        uv_sleep(1E2);
      } while(!done);

      log("onwork end work");
      return chunks;
    }

    void ondone(int* result, int status) {
      ASSERT(status == 0);
      log("ondone");
      fprintf(stderr, "processed %d chunks\n", *result);
    }
};

static void onloopIteration(uv_idle_t* handle) {
  loop_work_t* loop_work = static_cast<loop_work_t*>(handle->data);

  uv_mutex_lock(&loop_work->mutex);
  {
    log("loop {");
    std::vector<char> buffer(loop_work->chunkSize, 0);
    loop_work->stream.read(buffer.data(), buffer.size());
    loop_work->chunk = copy_buffer(buffer.data(), buffer.size());
    loop_work->chunkToProcess = true;

    log(" added chunk, waiting to process");

    if (loop_work->stream.eof()) {
      log(" reached end of stream, stopping");
      loop_work->done = true;
      uv_idle_stop(handle);
    } else {

    }
  }
  uv_mutex_unlock(&loop_work->mutex);
  log("}");
}

int main(int argc, char *argv[]) {
  int r;
  uv_loop_t* loop = uv_default_loop();
  const char* file = argv[0];

  // Initialize Producer that adds a chunk to be processed on each loop
  uv_idle_t idle;
  r = uv_idle_init(loop, &idle);
  ASSERT(r == 0);

  std::ifstream stream(file);
  loop_work_t loop_work = {
    .stream = stream,
    .chunkSize = CHUNK_SIZE,
    .chunkToProcess = true,
    .done = false
  };

  idle.data = (void*)(&loop_work);
  r = uv_mutex_init(&loop_work.mutex);
  ASSERT(r == 0);

  r = uv_idle_start(&idle, onloopIteration);
  ASSERT(r == 0);

  // Initialize Worker that will process the produced chunks in the background
  ChunkProcessor processor(loop, loop_work);
  r = processor.work();
  ASSERT(r == 0);

  log("Starting loop");
  uv_run(loop, UV_RUN_DEFAULT);
}
