export module scheduler.task.promise;
import <coroutine>;
import <memory>;
import <vector>;
import <concepts>;
import <optional>;
import <stdexcept>;
import <iostream>;
import <chrono>;

import scheduler.task.tasktype;
import scheduler.task.task;
import scheduler.task.final_awaiter;

using namespace std::chrono;

export template <typename T, TaskType task_type>
class promise
{
    template <typename U, TaskType tt2>
    friend class base_task;
    friend class final_awaiter;

    std::optional<T> value{};

    std::shared_ptr<std::vector<std::pair<void*, TaskType>>> recursive_info;
public:
    auto get_return_object() noexcept { return base_task<T, task_type>{ std::coroutine_handle<promise>::from_promise(*this) }; }

    std::suspend_always initial_suspend() noexcept
    {
        recursive_info = std::make_shared<std::vector<std::pair<void*, TaskType>>>();
        recursive_info->push_back({ std::coroutine_handle<promise>::from_promise(*this).address(), task_type });
        return {};
    }

    final_awaiter final_suspend() noexcept { return {}; }

    final_awaiter yield_value(T&& t)
    {
        value = std::move(t);
        return {};
    }

    final_awaiter yield_value(T& t)
    {
        value = t;
        return {};
    }

    void return_value(T&& t)
    {
        value = std::move(t);
    }

    void return_value(T& t)
    {
        value = t;
    }

    void unhandled_exception() noexcept
    {
        try
        {
            std::rethrow_exception(std::current_exception());
        }
        catch (const std::exception& e)
        {
            std::cerr << "Caught exception: '" << e.what() << "'\n";
        }
        std::terminate();
    }
};

export template <TaskType task_type>
class promise<void, task_type>
{
    template <typename U, TaskType>
    friend class base_task;
    friend class final_awaiter;

    std::shared_ptr<std::vector<std::pair<void*, TaskType>>> recursive_info;
public:
    auto get_return_object() noexcept { return base_task<void, task_type>{ std::coroutine_handle<promise>::from_promise(*this) }; }

    std::suspend_always initial_suspend() noexcept
    {
        recursive_info = std::make_shared<std::vector<std::pair<void*, TaskType>>>();
        recursive_info->push_back({ std::coroutine_handle<promise>::from_promise(*this).address(), task_type });
        return {};
    }

    final_awaiter final_suspend() noexcept { return {}; }

    void return_void() noexcept {}

    void unhandled_exception() noexcept {
        try
        {
            std::rethrow_exception(std::current_exception());
        }
        catch (const std::exception& e)
        {
            std::cerr << "Caught exception: '" << e.what() << "'\n";
        }
        std::terminate();
    }
};

export template <>
class promise<milliseconds, TaskType::SLEEP>
{
    template <typename U, TaskType tt2>
    friend class base_task;
    friend class final_awaiter;

    friend class EventLoop;
    std::optional<milliseconds> value{};

    std::shared_ptr<std::vector<std::pair<void*, TaskType>>> recursive_info;
public:
    base_task<milliseconds, TaskType::SLEEP> get_return_object() noexcept
    {
        auto ch = std::coroutine_handle<promise>::from_promise(*this);
        return base_task<milliseconds, TaskType::SLEEP>(ch);
    }

    std::suspend_always initial_suspend() noexcept
    {
        recursive_info = std::make_shared<std::vector<std::pair<void*, TaskType>>>();
        recursive_info->push_back({ std::coroutine_handle<promise>::from_promise(*this).address(), TaskType::SLEEP });
        return {};
    }

    final_awaiter final_suspend() noexcept { return {}; }

    final_awaiter yield_value(milliseconds t)
    {
        value = t;
        return {};
    }

    void return_value(milliseconds t)
    {
        value = t;
    }

    void unhandled_exception() noexcept
    {
        try
        {
            std::rethrow_exception(std::current_exception());
        }
        catch (const std::exception& e)
        {
            std::cerr << "Caught exception: '" << e.what() << "'\n";
        }
        std::terminate();
    }
};
