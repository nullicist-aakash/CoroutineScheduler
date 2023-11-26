module;
#include <format>
#include <stdio.h>
#include "network_import.h"

export module network.udp;
import network.types;
import <utility>;
import <string>;

constexpr int READ_MAX_SIZE = 65535;

export class UDP
{
#if WINDOWS
	SOCKET sockfd{ INVALID_SOCKET };
#elif UNIX
#define	INVALID_SOCKET -1
#define SOCKET_ERROR -1
	int sockfd{ INVALID_SOCKET };
#endif

public:
	UDP()
	{
#if WINDOWS
		WSAWrapper::instance();
#endif
		sockfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sockfd == INVALID_SOCKET)
			throw std::runtime_error(std::format("'socket' error while opening UDP socket: {}", get_err_str()));
	}

	UDP(const Socket& self_socket) : UDP() { this->bind(self_socket); }
	UDP(const PORT& self_port) : UDP(Socket{ {}, self_port }) {}
	UDP(UDP&) = delete;
	UDP& operator=(UDP& other) = delete;

	UDP(UDP&& other)
	{
		this->sockfd = other.sockfd;
		other.sockfd = INVALID_SOCKET;
	}

	UDP& operator=(UDP&& other)
	{
		if (this == &other)
			return *this;

		this->sockfd = other.sockfd;
		other.sockfd = INVALID_SOCKET;
		return *this;
	}

	void bind(const Socket& self_socket)
	{
		struct sockaddr_in serv_addr = self_socket;

		if (::bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
			throw std::runtime_error(std::format("'bind' error while binding UDP socket on {}: {}", (std::string)self_socket, get_err_str()));
	}

	void send(const std::string& sv, const Socket& remote) const
	{
		sockaddr_in serv_addr = remote;
		int bytes_sent = sendto(sockfd, sv.data(), (int)sv.size(), 0, (sockaddr*)&serv_addr, sizeof(serv_addr));
		if (bytes_sent == SOCKET_ERROR)
			throw std::runtime_error(std::format("'sendto' error while sending '{}' to {}: {}", sv, (std::string)remote, get_err_str()));
	}

	std::pair<std::string, Socket> receive() const
	{
		sockaddr_in serv_addr{};
		socklen_t len = sizeof(serv_addr);
		std::string str(READ_MAX_SIZE, '\0');
		int n = recvfrom(sockfd, &str[0], READ_MAX_SIZE, 0, (sockaddr*)&serv_addr, &len);

		if (n == SOCKET_ERROR)
			throw std::runtime_error(std::format("'recvfrom' error while receiving data on PORT {}: {}", (std::string)this->get_self_port(), get_err_str()));

		str.resize(n);
		return { str, Socket{ serv_addr } };
	}

	PORT get_self_port() const
	{
		sockaddr_in addr{};
		int len = sizeof(addr);
		if (::getsockname(sockfd, (sockaddr*)&addr, &len) == SOCKET_ERROR)
			throw std::runtime_error(std::format("'getsockname' error while fetching PORT info for UDP: {}", get_err_str()));
		return addr.sin_port;
	}

	void close()
	{
		if (sockfd == INVALID_SOCKET)
#if WINDOWS
			::closesocket(sockfd);
#elif UNIX
			::close(sockfd);
#endif
		sockfd = INVALID_SOCKET;
	}

	~UDP()
	{
		this->close();
	}
};