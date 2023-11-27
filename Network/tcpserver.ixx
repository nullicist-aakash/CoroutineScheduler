module;
#include "network_import.h"

export module network.tcp.server;
import network.types;
import network.tcp;
import <utility>;
import <string>;

export class TCPServer
{
	SOCKET_TYPE listenfd{ INVALID_SOCKET };
	Socket self_socket;
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