# Couroutine Scheduler

Here, I tried the implementation of nodejs scheduler in C++ where the entrie app is single threaded but I/O tasks run in background threads. 

To simulate the similar behavior, I used C++ coroutines and introduced my own versions of io and cpu-tasks.

I will continue working on this in future after re-learning I/O multiplexing in unix systems. There are some issues in the current implementation.
