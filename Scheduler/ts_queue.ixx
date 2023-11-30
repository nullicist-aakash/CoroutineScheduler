export module scheduler.ts_queue;
import <vector>;
import <utility>;
import <queue>;
import <mutex>;
import <condition_variable>;
import <chrono>;
import <concepts>;

using namespace std::chrono;

template <class T> requires std::movable<T>
class ex_pq;

template <typename T> requires std::movable<T>
class queue_obj
{
    friend class ex_pq<T>;
    time_point<steady_clock> end_time;
    T obj;

public:
    queue_obj(time_point<steady_clock> end_time, T&& obj) : end_time{ end_time }, obj{ std::move(obj) }
    {

    }

    queue_obj(queue_obj& rhs) = delete;
    queue_obj& operator=(queue_obj& rhs) = delete;
    queue_obj(queue_obj&& rhs) noexcept : end_time(rhs.end_time), obj(std::move(rhs.obj))
    {

    }
    queue_obj& operator=(queue_obj&& rhs) noexcept
    {
        end_time = rhs.end_time;
        obj = std::move(rhs.obj);
        return *this;
    }

    bool operator<(const queue_obj& rhs) const
    {
        return end_time < rhs.end_time;
    }

    bool operator>(const queue_obj& rhs) const
    {
        return end_time > rhs.end_time;
    }

    auto get_time() const
    {
        return end_time;
    }
};

template <class T> requires std::movable<T>
class ex_pq : public std::priority_queue<queue_obj<T>, std::vector<queue_obj<T>>, std::greater<queue_obj<T>>> 
{
public:
    T top_and_pop() 
    {
        std::pop_heap(c.begin(), c.end(), comp);
        T value = std::move(c.back().obj);
        c.pop_back();
        return value;
    }

protected:
    using Container = std::vector<queue_obj<T>>;
    using Compare = std::greater<queue_obj<T>>;
    using std::priority_queue<queue_obj<T>, Container, Compare>::c;
    using std::priority_queue<queue_obj<T>, Container, Compare>::comp;
};

export template <typename T> requires std::movable<T>
class threadsafe_queue
{
    ex_pq<T> queue;
    std::mutex mutex_;
    std::condition_variable cv;
public:
    void push(T&& val, milliseconds ms = 0)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue.emplace(steady_clock::now() + ms, std::move(val));
        cv.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (true)
        {
            if (queue.empty())
            {
                cv.wait(lock);
                continue;
            }

            // Sleep till we can execute top element
            auto cur_time = steady_clock::now();
            if (cur_time >= queue.top().get_time())
                break;

            cv.wait_until(lock, queue.top().get_time());
        }

        return queue.top_and_pop(); // Trick
    }

    bool empty()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue.empty();
    }
};