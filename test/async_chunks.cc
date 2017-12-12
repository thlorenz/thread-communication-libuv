#include "async_worker_base.cc"
#include "common.h"
#include <uv.h>

uint64_t START_TIME = uv_hrtime() / 1E6;
#define CHUNK_SIZE 640
#define SLEEP_TIME 40

typedef struct {
  char* data;
  size_t size;
} chunk_t;

typedef struct {
  std::istream& stream;
  size_t chunkSize;
  std::vector<chunk_t*> chunks;
  bool done;
  uv_mutex_t mutex;
} loop_work_t;

typedef struct {
  int32_t charCount;
  int32_t chunksProcessed;
} work_result_t;

class ChunkProcessor : public AsyncWorkerBase<work_result_t, loop_work_t> {
  public:
    ChunkProcessor(
        uv_loop_t* loop,
        loop_work_t& work,
        const char charToCount
      ) : AsyncWorkerBase(loop, work), charToCount_(charToCount) {}

  private:
    work_result_t* onwork(loop_work_t& work) {
      log("onwork start work");
      work_result_t* result = new work_result_t({
        .charCount = 0,
        .chunksProcessed = 0
      });

      bool done;
      bool chunkToProcess;
      do {
        chunk_t chunkcpy;
        uv_mutex_lock(&work.mutex);
        {
          log("processor reading {");
          done = work.done;
          chunkToProcess = work.chunks.size() > 0;
          if (chunkToProcess) {
            log("  will process new chunk");
            chunk_t* chunk = work.chunks.front();
            chunkcpy = { .data = chunk->data, .size = chunk->size };
            work.chunks.erase(work.chunks.begin());
          } else {
            log("  no chunk to process");
          }
        }
        uv_mutex_unlock(&work.mutex);
        log("}");

        if (chunkToProcess) {
          result->chunksProcessed++;
          result->charCount += count_char(chunkcpy.data, chunkcpy.size, charToCount_);

          // pretend counting chars takes a lot longer
          uv_sleep(SLEEP_TIME);
          delete [] chunkcpy.data;
        }
      } while(!done || chunkToProcess);

      log("onwork end work");
      return result;
    }

    void ondone(work_result_t* result, int status) {
      ASSERT(status == 0);
      log("ondone");
      fprintf(stderr, "processed %d chunks and found '%c' %d times\n",
          result->chunksProcessed, charToCount_, result->charCount);
      delete result;
    }

    const char charToCount_;
};

static void onloopIteration(uv_idle_t* handle) {
  loop_work_t* loop_work = static_cast<loop_work_t*>(handle->data);

  uv_mutex_lock(&loop_work->mutex);
  {
    log("loop {");
    std::vector<char> buffer(loop_work->chunkSize, 0);
    loop_work->stream.read(buffer.data(), buffer.size());
    chunk_t* chunk = new chunk_t({
      .data = copy_buffer(buffer.data(), buffer.size()),
      .size = buffer.size()
    });
    loop_work->chunks.emplace_back(chunk);

    log(" added chunk, waiting to process");

    if (loop_work->stream.eof()) {
      log(" reached end of stream, stopping");
      loop_work->done = true;
      uv_idle_stop(handle);
    }
  }
  uv_mutex_unlock(&loop_work->mutex);
  log("}");

  // producing chunks twice as fast as they are processed
  uv_sleep(SLEEP_TIME / 2);
}

int main(int argc, char *argv[]) {
  int r;
  uv_loop_t* loop = uv_default_loop();
  const char* file = argv[1];
  fprintf(stderr, "Processing %s\n", file);
  std::ifstream stream(file);

  // Initialize Producer that adds a chunk to be processed on each loop
  uv_idle_t idle;
  r = uv_idle_init(loop, &idle);
  ASSERT(r == 0);

  loop_work_t loop_work = {
    .stream = stream,
    .chunkSize = CHUNK_SIZE,
    .done = false,
  };

  idle.data = (void*)(&loop_work);
  r = uv_mutex_init(&loop_work.mutex);
  ASSERT(r == 0);

  r = uv_idle_start(&idle, onloopIteration);
  ASSERT(r == 0);

  // Initialize Worker that will process the produced chunks in the background
  ChunkProcessor processor(loop, loop_work, 'A');
  r = processor.work();
  ASSERT(r == 0);

  log("Starting loop");
  uv_run(loop, UV_RUN_DEFAULT);
}
