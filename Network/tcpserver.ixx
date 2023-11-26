module;
#include <format>
#include <stdio.h>
#include "network_import.h"
#include <system_error>

export module network.tcp.server;
import network.types;
import network.tcp.client;
import <utility>;
import <string>;

export class TCPServer
{
	static const int LISTEN_LEN = 5;
	int listenfd{ -1 };
	const Socket self_socket;
public:
	TCPServer(const Socket&);

	TCPServer(TCPServer&) = delete;
	TCPServer& operator=(TCPServer&) = delete;
	TCPServer(TCPServer&&);
	TCPServer& operator=(TCPServer&&);

	const Socket& get_socket() const;
	TCP accept() const;
	void close();

	~TCPServer();
};