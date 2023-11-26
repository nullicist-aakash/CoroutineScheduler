#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>
import network;
#include "network_import.h"

using namespace std;

int main()
{
	try
	{
		UDP udp{ Socket{ {}, PORT{8081, ByteOrder::HOST} } };
		while (true)
		{
			auto [msg, sender] = udp.receive();
			udp.send(msg, sender);
		}
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
	}
}