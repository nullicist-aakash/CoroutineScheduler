module;
#include <format>
#include <stdio.h>
#include "network_import.h"
#include <system_error>

export module network.udp;
import network.types;
import <utility>;
import <string>;

constexpr int READ_MAX_SIZE = 65535;

std::string get_err_str()
{
	DWORD error = ::GetLastError();
	return std::system_category().message(error);
}

#ifdef WINDOWS
export class UDP
{
	SOCKET sockfd{};
public:
	UDP()
	{
		WSAWrapper::instance();
		sockfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sockfd == INVALID_SOCKET)
			throw std::runtime_error(std::format("socket error while opening UDP socket: {}", get_err_str()));
	}

	UDP(const Socket& self_socket) : UDP()
	{
		this->bind(self_socket);
	}

	UDP(const PORT& self_port) : UDP(Socket{ {}, self_port }) 
	{
	}

	UDP(UDP&) = delete;
	UDP& operator=(UDP& other) = delete;
	UDP(UDP&& other)
	{
		WSAWrapper::instance();
		this->sockfd = other.sockfd;
		other.sockfd = -1;
	}
	UDP& operator=(UDP&& other)
	{
		if (this == &other)
			return *this;

		this->sockfd = other.sockfd;
		other.sockfd = -1;
		return *this;
	}

	void bind(const Socket& self_socket)
	{
		struct sockaddr_in serv_addr = self_socket;

		if (::bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
			throw std::runtime_error(std::format("'bind' error while binding UDP socket: {}", get_err_str()));
	}

	void send(const std::string& sv, const Socket& remote) const
	{
		sockaddr_in serv_addr = remote;
		int bytes_sent = sendto(sockfd, sv.data(), sv.size(), 0, (sockaddr*)&serv_addr, sizeof(serv_addr));
		if (bytes_sent == SOCKET_ERROR)
			throw std::runtime_error(std::format("'sendto' error while sending '{}' to {}: {}", sv, (std::string)remote, get_err_str()));
	}

	std::pair<std::string, Socket> receive() const
	{
		sockaddr_in serv_addr{};
		int len = sizeof(serv_addr);
		std::string str(READ_MAX_SIZE, '\0');

		int n = ::recvfrom(sockfd, &str[0], READ_MAX_SIZE, 0, (sockaddr*)&serv_addr, &len);

		if (n == SOCKET_ERROR)
			throw std::runtime_error(std::format("'recvfrom' error while receiving data on PORT {}: {}", (std::string)this->get_self_port(), get_err_str()));

		str.resize(n);
		return { str, Socket{ serv_addr } };
	}
	PORT get_self_port() const
	{
		SOCKADDR_IN addr{};
		int len = sizeof(addr);

		if (::getsockname(sockfd, (struct sockaddr*)&addr, &len) == SOCKET_ERROR)
			throw std::runtime_error(std::format("'getsockname' error while fetching PORT info for UDP: {}", get_err_str()));
		return addr.sin_port;
	}

	void close()
	{
		if (sockfd > -1)
			::closesocket(sockfd);
		sockfd = -1;
	}

	~UDP()
	{
		this->close();
	}
};
#elif UNIX
export class UDP
{
	int sockfd{ -1 };
public:
	UDP()
	{
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		if (sockfd < 0)
			throw std::runtime_error(std::format("'socket' error while opening UDP socket: {}", strerror(errno)));
	}
	UDP(const Socket& self_socket)
	{
		this->bind(self_socket);
	}
	UDP(const PORT& self_port) : UDP(Socket{ {}, self_port }) {}

	UDP(UDP&) = delete;
	UDP& operator=(UDP& other) = delete;
	UDP(UDP&& other)
	{
		this->sockfd = other.sockfd;
		other.sockfd = -1;
	}

	UDP& operator=(UDP&& other)
	{
		if (this == &other)
			return *this;

		this->sockfd = other.sockfd;
		other.sockfd = -1;
		return *this;
	}

	void bind(const Socket& self_socket)
	{
		struct sockaddr_in serv_addr = self_socket;

		if (::bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
			throw std::runtime_error(std::format("'bind' error while binding UDP socket on {}: {}", (string)self_socket, strerror(errno)));
	}

	void send(const std::string& sv, const Socket& remote) const
	{
		sockaddr_in serv_addr = remote;

		if (sendto(sockfd, sv.data(), sv.size(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
			throw std::runtime_error(std::format("'sendto' error while sending '{}' to {}: {}", sv, (string)remote, strerror(errno)));
	}

	std::pair<std::string, Socket> receive() const
	{
		sockaddr_in serv_addr{};
		socklen_t len = sizeof(serv_addr);
		std::string str(READ_MAX_SIZE, '\0');
		ssize_t n = 0;

		if ((n = recvfrom(sockfd, str.data(), READ_MAX_SIZE, 0, (sockaddr*)&serv_addr, &len)) < 0)
			throw std::runtime_error(std::format("'recvfrom' error while receiving data on PORT {}: {}", (string)this->get_self_port(), strerror(errno)));

		str.resize(n);
		return	{ str, Socket{ serv_addr } }; 
	}
	PORT get_self_port() const
	{
		sockaddr_in addr {};
		socklen_t len = sizeof(addr);
		if (getsockname(sockfd, (struct sockaddr*)&addr, &len) < 0)
			throw std::runtime_error(std::format("'getsockname' error while fetching PORT info for UDP: {}", strerror(errno)));
		return addr.sin_port;
	}

	void close()
	{
		if (sockfd > -1)
			::close(sockfd);
		sockfd = -1;
	}

	~UDP()
	{
		this->close();
	}
};
#else
#error "Unsupported platform"
#endif