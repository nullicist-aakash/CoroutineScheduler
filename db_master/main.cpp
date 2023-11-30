#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>
#include <chrono>
import network;
import scheduler;

using namespace std;
using namespace std::chrono;

sleep_task hoo(seconds s)
{
    co_return s;
}

task<void> foo(seconds x)
{
    co_await hoo(x);
    cout << x << endl;
}

int main()
{
    clog.setstate(std::ios_base::failbit);
    auto& el = EventLoop::get_instance();

    el.schedule(foo(6s));
    el.schedule(foo(4s));
    el.schedule(foo(3s));
    el.schedule(foo(1s));
    el.schedule(foo(7s));
    el.run();
}