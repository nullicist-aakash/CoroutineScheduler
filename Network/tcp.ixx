module;
#include <format>
#include <stdio.h>
#include "network_import.h"

export module network.tcp.client;
import network.types;
import <string>;

export class TCP
{
#if WINDOWS
	SOCKET sockfd{};
#elif UNIX
	int sockfd{ -1 };
#endif
	SocketPair socket_pair;

	friend class TCPServer;
	TCP(int, const SocketPair&);
public:
	TCP(const SocketPair&);

	TCP(TCP&) = delete;
	TCP& operator=(TCP&) = delete;
	TCP(TCP&&);
	TCP& operator=(TCP&&);

	int send(const std::string&) const;
	std::string receive(size_t n = 0) const;

	const SocketPair& get_socket_pair() const;
	void close();

	~TCP();
};