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

task<void> foo()
{
    Socket self = { IP{"127.0.0.1"}, PORT{15608, ByteOrder::HOST} };
    Socket remote = { IP{"127.0.0.1"}, PORT{8081, ByteOrder::HOST} };
    SocketPair sp = { self, remote };
    TCP tcp{ sp };
    while (true)
    {
        auto res = co_await tcp.receive();
        cout << res << endl;
        co_await hoo(5s);
        co_await tcp.send(res);
    }
    co_return;
}

int main()
{
    clog.setstate(std::ios_base::failbit);

    auto& el = EventLoop::get_instance();
    el.schedule(foo());
    el.run();
}