#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>
#include <chrono>
import network;
import scheduler.ts_queue;

using namespace std;
using namespace std::chrono;

class X
{
	int x = 0;
public:
	X(int x) : x{ x }
	{

	}

	X(X& rhs) = delete;
	X& operator=(X& rhs) = delete;

	X(X&& rhs) noexcept : x{ rhs.x }
	{
		cout << "Move constructor called" << endl;
	}
	X& operator=(X&& rhs) noexcept
	{
		cout << "Move assignment called" << endl;
		x = rhs.x;
		return *this;
	}

	friend ostream& operator<<(ostream& os, const X& rhs)
	{
		os << rhs.x;
		return os;
	}
};

int main()
{
	threadsafe_queue<X> q;
	q.push(X{ 1 }, 1s);
	q.push(X{ 0 }, 0s);
	while (!q.empty())
		cout << q.pop() << endl;
}