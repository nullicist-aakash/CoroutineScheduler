module;
#include "network_import.h"

export module network.tcp;
import network.types;
import scheduler;
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
	TCP(TCP&&) noexcept;
	TCP& operator=(TCP&&) noexcept;

	io_task<size_t> send(std::string_view) const;
	io_task<std::string> receive(size_t n = 0) const;

	const SocketPair& get_socket_pair() const;
	io_task<void> close();

	~TCP();
};