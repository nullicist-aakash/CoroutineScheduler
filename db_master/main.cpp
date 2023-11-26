#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>

import network;
using namespace std;

int main()
{
	UDP udp;
	udp.bind(Socket{ {}, PORT{8080, ByteOrder::HOST} });
	while (true)
	{
		auto [data, addr] = udp.receive();
		cout << format("Received {} bytes from {}: {}\n", data.size(), (string)addr, data);
		udp.send(data, addr);
	}
}