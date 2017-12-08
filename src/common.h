#ifndef SRC_COMMON_H
#define SRC_COMMON_H

#include <stdlib.h>
#include <unistd.h>
#include <uv.h>

#include <vector>
#include <iostream>
#include <fstream>

extern uint64_t START_TIME;

/* assert lifted from libuv itself */
#define ASSERT(expr)                                      \
 do {                                                     \
  if (!(expr)) {                                          \
    fprintf(stderr,                                       \
            "Assertion failed in %s on line %d: %s\n",    \
            __FILE__,                                     \
            __LINE__,                                     \
            #expr);                                       \
    abort();                                              \
  }                                                       \
 } while (0)

void inline uv_sleep(int msec) {
  int sec;
  int usec;

  sec = msec / 1000;
  usec = (msec % 1000) * 1000;
  if (sec > 0)
    sleep(sec);
  if (usec > 0)
    usleep(usec);
}

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') \
    ? __builtin_strrchr(__FILE__, '/') + 1 \
    : __FILE__)

#define log(msg)                                      \
  do {                                                \
    uint64_t time = (uv_hrtime() / 1E6) - START_TIME; \
    fprintf(stderr,                                   \
            "[%05lld](%s:%d)[0x%lx] %s\n",            \
            time,                                     \
            __FILENAME__,                             \
            __LINE__,                                 \
            (unsigned long int) uv_thread_self(),     \
            msg);                                     \
  } while (0)

inline int* count_chunks(char& file, size_t chunkSize) {
  std::ifstream stream(&file);

  int* chunks = new int(0);
  do {
    std::vector<char> buffer (chunkSize, 0);
    stream.read(buffer.data(), buffer.size());
    usleep(1E5);
    (*chunks)++;
    log("processed chunk");
  } while(!stream.eof());
  return chunks;
}

#endif
