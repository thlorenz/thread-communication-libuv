UV_PATH     = $(DEPS)/uv
UV_LIB      = $(UV_PATH)/out/Debug/libuv.a
UV_INCLUDES = $(UV_PATH)/include/
UV_SRCS     = $(wildcard $(UV_PATH)/src/*.c) $(wildcard $(UV_PATH)/src/unix/*.c)
UV_FLAGS    = -pthread

uname_S=$(shell uname -s)
LIBS=
ifeq (Linux, $(uname_S))
LIBS=-lrt -ldl -lm -pthread
endif

$(UV_PATH):
	git clone git@github.com:libuv/libuv.git $@ && \
	cd $(UV_PATH) &&                               \
	git checkout tags/v1.16.1

$(UV_LIB): $(UV_PATH)
	cd $(UV_PATH) &&                                                                                            \
    (test -d ./build/gyp ||                                                                                     \
    	(mkdir -p ./build && git clone https://chromium.googlesource.com/external/gyp $(UV_PATH)/build/gyp)) && \
	./gyp_uv.py -f make &&                                                                                      \
	make -C out -j8
