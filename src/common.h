#ifndef SRC_COMMON_H
#define SRC_COMMON_H

#include <stdlib.h>
#include <unistd.h>
#include <uv.h>

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

#define log(msg)                                  \
  do {                                            \
    double uptime;                                \
    uv_uptime(&uptime);                           \
    fprintf(stderr,                               \
            "[%f](%s:%d)[0x%lx] %s\n",            \
            uptime,                               \
            __FILENAME__,                         \
            __LINE__,                             \
            (unsigned long int) uv_thread_self(), \
            msg);                                 \
  } while (0)

#endif 
