# thread-communication-libuv

Small examples related to scheduling work and communicating results + process across threads with libuv

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**  *generated with [DocToc](https://github.com/thlorenz/doctoc)*

- [Examples](#examples)
  - [async_queue](#async_queue)
  - [async_worker](#async_worker)
  - [async_chunks](#async_chunks)
- [Xcode](#xcode)
- [LICENSE](#license)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Examples

All examples use a special `log` function to print the following information to give insight into what is happening when
and on which thread.

```
Format:
  [Milliseconds since program start](file-name:line-number)[thread id] <message>

Example:
  [00002](async_queue.cc:39)[0x7fff89508340] Starting loop
```

### [async_queue](test/async_queue.cc)

Demonstrates how to use `uv_queue_work` to schedule work on a background thread.

```sh
make clean && make async_queue && ./bin/async_queue
```

```
[00001](async_queue.cc:35)[0x7fff89508340] Queueing background work
[00002](async_queue.cc:39)[0x7fff89508340] Starting loop
[00002](async_queue.cc:16)[0x7000021a2000] Starting queued background work
[00203](async_queue.cc:18)[0x7000021a2000] Finishing queued background work
[00203](async_queue.cc:25)[0x7fff89508340] Finished queued background work
```

### [async_worker](test/async_worker.cc)

Demonstrates use of `AsyncWorker`, a light wrapper around `uv_async_queue` to schedule async work in the background.
For more information see the base [header](src/async_worker_base.h) and [implementation](src/async_worker_base.cc) files.

```sh
make clean && make async_worker && ./bin/async_worker
```

```
[00002](async_worker.cc:51)[0x7fff89508340] Starting loop
[00002](async_worker.cc:20)[0x700002ff9000] onwork start
[00105](common.h:65)[0x700002ff9000] processed chunk
[00211](common.h:65)[0x700002ff9000] processed chunk
[00317](common.h:65)[0x700002ff9000] processed chunk
[00423](common.h:65)[0x700002ff9000] processed chunk
[00528](common.h:65)[0x700002ff9000] processed chunk
[00528](async_worker.cc:22)[0x700002ff9000] onwork end
[00529](async_worker.cc:27)[0x7fff89508340] ondone, calling final done
[00529](async_worker.cc:37)[0x7fff89508340] onfinalDone
read 5 chunks
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

```
[00001](async_chunks.cc:148)[0x7fff89508340] Starting loop
[00001](async_chunks.cc:37)[0x700002ecc000] onwork start work
[00001](async_chunks.cc:94)[0x7fff89508340] loop {
[00001](async_chunks.cc:103)[0x7fff89508340]  added chunk, waiting to process
[00001](async_chunks.cc:112)[0x7fff89508340] }
[00001](async_chunks.cc:49)[0x700002ecc000] processor reading {
[00001](async_chunks.cc:53)[0x700002ecc000]   will process new chunk
[00001](async_chunks.cc:62)[0x700002ecc000] }
[00022](async_chunks.cc:94)[0x7fff89508340] loop {
[00022](async_chunks.cc:103)[0x7fff89508340]  added chunk, waiting to process
[00022](async_chunks.cc:112)[0x7fff89508340] }
[00046](async_chunks.cc:49)[0x700002ecc000] processor reading {
[00046](async_chunks.cc:53)[0x700002ecc000]   will process new chunk
[00046](async_chunks.cc:62)[0x700002ecc000] }
[00046](async_chunks.cc:94)[0x7fff89508340] loop {
[00046](async_chunks.cc:103)[0x7fff89508340]  added chunk, waiting to process
[00046](async_chunks.cc:106)[0x7fff89508340]  reached end of stream, stopping
[00046](async_chunks.cc:112)[0x7fff89508340] }
[00094](async_chunks.cc:49)[0x700002ecc000] processor reading {
[00094](async_chunks.cc:53)[0x700002ecc000]   will process new chunk
[00094](async_chunks.cc:62)[0x700002ecc000] }
[00137](async_chunks.cc:49)[0x700002ecc000] processor reading {
[00137](async_chunks.cc:58)[0x700002ecc000]   no chunk to process
[00137](async_chunks.cc:62)[0x700002ecc000] }
[00137](async_chunks.cc:74)[0x700002ecc000] onwork end work
[00137](async_chunks.cc:80)[0x7fff89508340] ondone
processed 3 chunks and found 'A' 34 times
```

## Xcode

If you're on OSX and want to have an easier time debugging I recommend you to build and open the Xcode project as
follows.

```sh
make xcode && open ./build/thread_communication.xcodeproj
```

For more info on how this Xcode project is generated please see the [Makefile](Makefile) and the
[CMakeLists.txt](CMakelists.txt).

## LICENSE

MIT
