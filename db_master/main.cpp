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
	
	try
	{
		TCP tcp{ SocketPair{ self, remote } };
		while (true)
		{
			auto msg = tcp.receive();
			cout << msg << endl;
			if (msg == "exit")
				break;
			tcp.send(msg);
		}
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}