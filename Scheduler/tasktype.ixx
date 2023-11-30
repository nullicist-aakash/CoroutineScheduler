export module scheduler.task.tasktype;

import <coroutine>;
import <memory>;
import <vector>;

export enum class TaskType
{
    IO,
    CPU,
    SLEEP
};

export template <typename PROMISE>
concept is_promise = requires(PROMISE promise)
{
    { promise.recursive_info } ->  std::convertible_to<std::shared_ptr<std::vector<std::pair<void*, TaskType>>>>;
};
