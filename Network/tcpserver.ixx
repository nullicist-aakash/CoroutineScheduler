module;
#include "network_import.h"

export module network.tcp.server;
import network.types;
import network.tcp;
import scheduler;
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
	TCPServer(TCPServer&&) noexcept;
	TCPServer& operator=(TCPServer&&) noexcept;

	const Socket& get_socket() const;
	io_task<TCP> accept() const;
	io_task<void> close();

	~TCPServer();
};