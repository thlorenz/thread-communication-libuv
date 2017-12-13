ASYNC_WORKER_PATH     = $(DEPS)/async-worker.h
ASYNC_WORKER_INCLUDES = $(ASYNC_WORKER_PATH)/

$(ASYNC_WORKER_PATH):
	git clone git@github.com:nodesource/async-worker.h $@ && \
	cd $(ASYNC_WORKER_PATH)
