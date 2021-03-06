cmake_minimum_required(VERSION 3.9)

project(thread_communication)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_TEST_DIR ${CMAKE_SOURCE_DIR}/test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -g")
set(CMAKE_*_LINKER_FLAGS " -v ${UV_LIB}")

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_TEST_DIR})

# srcs and tests
file(GLOB thread_communication_headers ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB thread_communication_test_headers ${PROJECT_TEST_DIR}/*.h)
file(GLOB thread_communication_test_sources ${PROJECT_TEST_DIR}/*.cc)
source_group(src FILES ${thread_communication_headers})
source_group(test FILES ${thread_communication_test_sources} ${thread_communication_test_headers})

# libuv
include_directories(${PROJECT_SOURCE_DIR}/../deps/uv/include)
file(GLOB uv_headers ${PROJECT_SOURCE_DIR}/../deps/uv/include/*)
file(GLOB uv_sources ${PROJECT_SOURCE_DIR}/../deps/uv/src/*)
source_group(deps\\uv\\include FILES ${uv_headers})
source_group(deps\\uv\\src FILES ${uv_sources})

# async worker
include_directories(${PROJECT_SOURCE_DIR}/../deps/async-worker.h)
file(GLOB async_worker_headers ${PROJECT_SOURCE_DIR}/../deps/async-worker.h/*.h)
source_group(deps\\async-worker.h FILES ${async_worker_headers})

add_executable(async_queue
  ${thread_communication_headers}
  ${PROJECT_TEST_DIR}/async_queue.cc ${thread_communication_test_headers}
  ${uv_sources} ${uv_unix_sources} ${uv_headers}
)

add_executable(async_worker
  ${thread_communication_headers}
  ${PROJECT_TEST_DIR}/async_worker.cc ${thread_communication_test_headers}
  ${uv_sources} ${uv_unix_sources} ${uv_headers}
  ${async_worker_headers}
)

add_executable(async_chunks
  ${thread_communication_headers}
  ${PROJECT_TEST_DIR}/async_chunks.cc ${thread_communication_test_headers}
  ${uv_sources} ${uv_unix_sources} ${uv_headers}
  ${async_worker_headers}
)
