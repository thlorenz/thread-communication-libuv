# thread-communication-libuv

Small examples related to scheduling work and communicating results + process across threads with libuv

## Examples

### [async_queue](test/async_queue.cc)

Demonstrates how to use `uv_queue_work` to schedule work on a background thread.

```sh
make clean && make async_queue && ./bin/async_queue
```

### [async_worker](test/async_worker.cc)

Demonstrates use of `AsyncWorker`, a light wrapper around `uv_async_queue` to schedule async work in the background.
For more information see the base [header](src/async_worker_base.h) and [implemntation](src/async_worker_base.cc) files.

```sh
make clean && make async_worker && ./bin/async_worker
```

### [async_chunks](test/async_chunks.cc)

Up until now the work on the background thread was performed in isolation. This example shares data with the main loop
and the background worker.

It demonstrates the use of `uv_idle` in order to perform chunks of work on each loop iteration. In our case we produce
more chunks at that point. These chunks then are processed by the background AsyncWorker.

To that end we need to share data (the chunks and the state regarding if more chunks need processing) with the main loop
and the background thread where the AsyncWorker executes.
We orchestrate cross thread access to that data via a `uv_mutex_t`.

To count the number of `'A'` letters in the `Makefile` you'd do:

```sh
make clean && make async_chunks && ./bin/async_chunks ./Makefile
```

## Xcode

If you're on OSX and want to have an easier time debugging I recommend you to build and open the Xcode project as
follows.

```sh
make xcode && open ./build/thread_communication.xcodeproj`
```

For more info on how this Xcode project is generated please see the [Makefile](Makefile) and the
[CMakeLists.txt](CMakelists.txt).

## LICENSE

MIT
