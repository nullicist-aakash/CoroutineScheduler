module;
#include "network_import.h"

export module network.tcp;
import network.types;
import <string>;

export class TCP
{
	SOCKET_TYPE sockfd{ INVALID_SOCKET };
	SocketPair socket_pair{};

	friend class TCPServer;
	TCP(SOCKET_TYPE, const SocketPair&);
public:
	TCP(const SocketPair&);
	TCP(TCP&) = delete;
	TCP& operator=(TCP&) = delete;
	TCP(TCP&&);
	TCP& operator=(TCP&&);

	size_t send(std::string_view) const;
	std::string receive(size_t n = 0) const;

	const SocketPair& get_socket_pair() const;
	void close();

	~TCP();
};