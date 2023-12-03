module;
#include "network_import.h"

export module network.udp;

import network.types;
import scheduler;
import <utility>;
import <string>;

export class UDP
{
	SOCKET_TYPE sockfd{ INVALID_SOCKET };
public:
	UDP();
	UDP(const Socket&);
	UDP(const PORT&);
	UDP(UDP&) = delete;
	UDP& operator=(UDP&) = delete;
	UDP(UDP&&) noexcept;
	UDP& operator=(UDP&&) noexcept;

	io_task<void> bind(const Socket& self_socket);
	io_task<void> send(std::string_view sv, const Socket& remote) const;
	io_task<std::pair<std::string, Socket>> receive() const;
	PORT get_self_port() const;
	io_task<void> close();
	~UDP();
};