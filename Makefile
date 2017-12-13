ROOT        = $(shell pwd)
DEPS        = $(ROOT)/deps

-include libuv.mk
-include async-worker.mk

CCFLAGS = $(UV_FLAGS) -Wno-format -std=c++11 -g

SRC_DIR = $(ROOT)/src
TST_DIR = $(ROOT)/test
BIN_DIR = $(ROOT)/bin

INCS =-I$(UV_INCLUDES) -I$(ASYNC_WORKER_INCLUDES) -I$(SRC_DIR)

xcode:
	@mkdir -p build
	(cd build && rm -rf CMakeCache.txt CMakeFiles && \
		CC=`xcrun -find cc` CXX=`xcrun -find c++` LDFLAGS='$(UV_LIB)' cmake -G Xcode ..)

##
# Projects
##

# async_queue: queeing background work

ASYNC_QUEUE = $(BIN_DIR)/async_queue
ASYNC_QUEUE_SRCS=$(TST_DIR)/async_queue.cc
ASYNC_QUEUE_OBJS=$(ASYNC_QUEUE_SRCS:.cc=.o)

$(ASYNC_QUEUE): $(UV_LIB) $(ASYNC_QUEUE_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LIBS) $(ASYNC_QUEUE_OBJS) $(UV_LIB) -o $@

async_queue: $(ASYNC_QUEUE)

# async_worker: worker API using queueing under the hood

ASYNC_WORKER = $(BIN_DIR)/async_worker
ASYNC_WORKER_SRCS=$(TST_DIR)/async_worker.cc
ASYNC_WORKER_OBJS=$(ASYNC_WORKER_SRCS:.cc=.o)

$(ASYNC_WORKER): $(UV_LIB) $(ASYNC_WORKER_PATH) $(ASYNC_WORKER_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LIBS) $(ASYNC_WORKER_OBJS) $(UV_LIB) -o $@

async_worker: $(ASYNC_WORKER)

# async_chunks: worker API using queueing under the hood, sending providing work in chunks

ASYNC_CHUNKS = $(BIN_DIR)/async_chunks
ASYNC_CHUNKS_SRCS=$(TST_DIR)/async_chunks.cc
ASYNC_CHUNKS_OBJS=$(ASYNC_CHUNKS_SRCS:.cc=.o)

$(ASYNC_CHUNKS): $(UV_LIB) $(ASYNC_WORKER_PATH) $(ASYNC_CHUNKS_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LIBS) $(ASYNC_CHUNKS_OBJS) $(UV_LIB) -o $@

async_chunks: $(ASYNC_CHUNKS)

.SUFFIXES: .cc .o
.cc.o:
	$(CXX) $< $(CCFLAGS) $(INCS) -c -o $@

clean:
	@rm -f $(ASYNC_QUEUE) $(ASYNC_QUEUE_OBJS) \
		$(ASYNC_WORKER) $(ASYNC_WORKER_OBJS) \
		$(ASYNC_CHUNKS) $(ASYNC_CHUNKS_OBJS)

.PHONY: async_queue xcode
