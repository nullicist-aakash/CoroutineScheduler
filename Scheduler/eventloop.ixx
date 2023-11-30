export module scheduler.task.eventloop;

import <vector>;
import <utility>;
import <thread>;
import <coroutine>;
import <cassert>;
import <chrono>;

using namespace std::chrono;

import scheduler.ts_queue;
import scheduler.task.tasktype;
import scheduler.task.task;

export class EventLoop
{
    threadsafe_queue<task<void>> queue_for_loop;
    threadsafe_queue<task<void>> queue_for_io;
    const int num_threads;
    std::vector<std::thread> threads;

    void thread_task()
    {
        task<void> _task = std::move(queue_for_io.pop());  // blocks if no more task is available
        int counter = 0;
        std::vector<void*> io_coroutines;

        while (true)
        {
            auto [target_coroutine, ttype] = _task.get_handle_to_resume();

            void* addr = target_coroutine.address();
            bool in_set = std::find(io_coroutines.begin(), io_coroutines.end(), addr) != io_coroutines.end();
            bool is_io_sleep_task = ttype == TaskType::SLEEP || ttype == TaskType::IO;

            if (!in_set && is_io_sleep_task)
            {
                ++counter;
                io_coroutines.push_back(addr);
            }
            else if (!is_io_sleep_task || in_set)
            {
                io_coroutines.pop_back();
                --counter;
            }

            if (counter == 0)
            {
                queue_for_loop.push(std::move(_task));
                break;
            }

            assert(addr == io_coroutines.back());

            if (ttype == TaskType::IO)
                target_coroutine.resume();
            else
            {
                auto sleep_task = _task.get_handle_to_resume().first;
                sleep_task.resume();
                auto address = sleep_task.address();
                auto coroutine = std::coroutine_handle<promise<milliseconds, TaskType::SLEEP>>::from_address(address);
                milliseconds ms_time = *coroutine.promise().value;
                std::this_thread::sleep_for(ms_time);
            }

            if (_task.get_handles_count() == 0)
                throw std::logic_error("Developer Bug. I/O task should not finish all 'calls' here.");
        }
    }

    EventLoop() : num_threads{ 10 }
    {
        for (int i = 0; i < num_threads; ++i)
            threads.emplace_back([&]() { while (true) thread_task(); });
    }

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    EventLoop(EventLoop&&) = delete;
    EventLoop& operator=(EventLoop&&) = delete;

public:
    static EventLoop& get_instance()
    {
        static EventLoop instance;
        return instance;
    }

    void run()
    {
        while (true)
        {
            task<void> _task = std::move(queue_for_loop.pop());  // blocks if no more task is available
            auto [target_coroutine, is_io] = _task.get_handle_to_resume();

            target_coroutine.resume();

            if (_task.get_handles_count() == 0)
                continue;

            if (_task.get_handle_to_resume().second == TaskType::IO)
            {
                queue_for_io.push(std::move(_task));    // perform the task in thread pool
                continue;
            }
            else if (_task.get_handle_to_resume().second == TaskType::SLEEP)
            {
                auto sleep_task = _task.get_handle_to_resume().first;
                sleep_task.resume();
                auto address = sleep_task.address();
                auto coroutine = std::coroutine_handle<promise<milliseconds, TaskType::SLEEP>>::from_address(address);
                queue_for_loop.push(std::move(_task), *coroutine.promise().value);
            }
            else
                queue_for_loop.push(std::move(_task));
        }
    }

    void schedule(task<void>&& task)
    {
        queue_for_loop.push(std::move(task));
    }
};