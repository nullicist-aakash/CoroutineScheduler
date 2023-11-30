export module scheduler.task.task;
import <coroutine>;
import <memory>;
import <vector>;
import <concepts>;
import <optional>;
import <stdexcept>;
import <chrono>;
import <iostream>;

import scheduler.task.tasktype;
import scheduler.task.final_awaiter;

template <typename T, TaskType task_type>
class promise;

template <typename T, TaskType task_type>
class base_task
{
    template <typename U, TaskType tt2>
    friend class promise_type;

    friend class EventLoop;
public:
    using promise_type = ::promise<T, task_type>;
    explicit base_task(std::coroutine_handle<promise_type> h) noexcept
        : coro_(h)
    {}

    base_task(base_task& t) = delete;
    base_task& operator=(base_task& t) = delete;
    base_task(base_task&& t) noexcept : coro_(std::exchange(t.coro_, {}))
    {

    }
    base_task& operator=(base_task&& t) noexcept
    {
        coro_ = std::exchange(t.coro_, {});
        return *this;
    }

    ~base_task()
    {
        if (coro_)
            coro_.destroy();
    }

    bool await_ready() noexcept
    {
        return false;
    }

    template <typename U>
    std::coroutine_handle<> await_suspend(std::coroutine_handle<U> previous) noexcept
    {
        auto& previous_promise = previous.promise();
        auto& cur_promise = coro_.promise();

        void* prev_addr = previous.address();
        void* cur_addr = coro_.address();
        cur_promise.recursive_info = previous_promise.recursive_info;
        cur_promise.recursive_info->push_back({ cur_addr, task_type });

        if constexpr (task_type != TaskType::CPU)
            return std::noop_coroutine();
        return coro_;
    }

    T await_resume()
    {
        if constexpr (std::is_void_v<T>)
            return;
        else
        {
            if (!coro_.promise().value)
                throw std::runtime_error("Callee returned without yielding anything.");

            auto val = std::move(*coro_.promise().value);
            coro_.promise().value = std::nullopt;
            return val;
        }
    }

private:
    std::pair<std::coroutine_handle<>, TaskType> get_handle_to_resume()
    {
        auto& info = coro_.promise().recursive_info;
        return { std::coroutine_handle<>::from_address(info->back().first), info->back().second };
    }

    std::size_t get_handles_count()
    {
        return coro_.promise().recursive_info->size();
    }

    std::coroutine_handle<promise_type> coro_;
};

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
class promise<int, TaskType::SLEEP>
{
    template <typename U, TaskType tt2>
    friend class base_task;
    friend class final_awaiter;

    friend class EventLoop;
    std::optional<int> value{};

    std::shared_ptr<std::vector<std::pair<void*, TaskType>>> recursive_info;
public:
    auto get_return_object() noexcept { return base_task<int, TaskType::SLEEP>{ std::coroutine_handle<promise>::from_promise(*this) }; }

    std::suspend_always initial_suspend() noexcept
    {
        recursive_info = std::make_shared<std::vector<std::pair<void*, TaskType>>>();
        recursive_info->push_back({ std::coroutine_handle<promise>::from_promise(*this).address(), TaskType::SLEEP });
        return {};
    }

    final_awaiter final_suspend() noexcept { return {}; }

    final_awaiter yield_value(int t)
    {
        value = t;
        return {};
    }

    void return_value(int t)
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

export template <typename T>
using task = base_task<T, TaskType::CPU>;

export template <typename T>
using io_task = base_task<T, TaskType::IO>;

export using sleep_task = base_task<int, TaskType::SLEEP>;