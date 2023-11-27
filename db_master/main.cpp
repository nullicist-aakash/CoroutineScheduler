#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>

import network;
using namespace std;

int main()
{
	Socket self{ {"127.0.0.1"}, PORT{8081, ByteOrder::HOST} };
	Socket remote{ {"127.0.0.1"}, PORT{8082, ByteOrder::HOST} };
	
	TCPServer server{self};
	while (true)
	{
		auto client = server.accept();
		std::thread([&](auto cl) {
			while (true)
			{
				auto msg = cl.receive();
				cout << cl.get_socket_pair() << ": " << msg << endl;
				cl.send(msg);
			}
		}, std::move(client)).detach();
	}
}