ROOT        = $(shell pwd)
DEPS        = $(ROOT)/deps

-include libuv.mk

CCFLAGS = $(UV_FLAGS) -std=c++11 -g

SRC_DIR = $(ROOT)/src
TST_DIR = $(ROOT)/test
BIN_DIR = $(ROOT)/bin

SRCS := $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(SRCS:.cc=.o)
INCS =-I$(UV_INCLUDES) -I$(SRC_DIR)

xcode:
	@mkdir -p build
	(cd build && rm -rf CMakeCache.txt CMakeFiles && \
		CC=`xcrun -find cc` CXX=`xcrun -find c++` LDFLAGS='$(UV_LIB)' cmake -G Xcode ..)

##
# Projects
##

# background worker not communicating at all

ASYNC_SEND = $(BIN_DIR)/async_send
ASYNC_SEND_SRCS=$(TST_DIR)/async_send.cc
ASYNC_SEND_OBJS=$(ASYNC_SEND_SRCS:.cc=.o)

$(ASYNC_SEND): $(UV_LIB) $(OBJS) $(ASYNC_SEND_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LIBS) $(OBJS) $(ASYNC_SEND_OBJS) $(UV_LIB) -o $@

async_send: $(ASYNC_SEND)

ASYNC_QUEUE = $(BIN_DIR)/async_queue
ASYNC_QUEUE_SRCS=$(TST_DIR)/async_queue.cc
ASYNC_QUEUE_OBJS=$(ASYNC_QUEUE_SRCS:.cc=.o)

$(ASYNC_QUEUE): $(UV_LIB) $(OBJS) $(ASYNC_QUEUE_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(LIBS) $(OBJS) $(ASYNC_QUEUE_OBJS) $(UV_LIB) -o $@

async_queue: $(ASYNC_QUEUE)

.SUFFIXES: .cc .o
.cc.o:
	$(CXX) $< $(CCFLAGS) $(INCS) -c -o $@

clean:
	@rm -f $(OBJS) \
		$(ASYNC_SEND) $(ASYNC_SEND_OBJS)

.PHONY: async_send xcode
