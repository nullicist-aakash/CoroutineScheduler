module;
#include "network_import.h"

export module network.udp;

import network.types;
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

	void bind(const Socket& self_socket);
	void send(std::string_view sv, const Socket& remote) const;
	std::pair<std::string, Socket> receive() const;
	PORT get_self_port() const;
	void close();
	~UDP();
};