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

task<int> get_int()
{
	co_return 42;
}

task<void> print_int()
{
	cout << (co_await get_int()) << endl;
}

int main()
{
	auto &el = EventLoop::get_instance();
	el.schedule(print_int());
	el.run();
}