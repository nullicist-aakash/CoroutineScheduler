export module scheduler.task.final_awaiter;
import <coroutine>;
import <memory>;
import <vector>;
import <concepts>;
import scheduler.task.tasktype;

export class final_awaiter
{
public:
    inline bool await_ready() noexcept
    {
        return false;
    }

    template <typename PROMISE> requires is_promise<PROMISE>
    std::coroutine_handle<void> await_suspend(std::coroutine_handle<PROMISE> h) noexcept
    {
        auto recursive_info = h.promise().recursive_info;
        assert(recursive_info->back().first == h.address());

        // Top is what we are returning from
        if (recursive_info->size() == 1 || recursive_info->back().second != TaskType::CPU)
        {
            recursive_info->pop_back();
            return std::noop_coroutine();
        }

        recursive_info->pop_back();
        return std::coroutine_handle<void>::from_address(recursive_info->back().first);
    }

    void await_resume() noexcept
    {

    }
};
