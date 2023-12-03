module;
#include <format>
#include <stdio.h>
#include "network_import.h"

module network.udp;
import network.types;
import scheduler;
import <utility>;
import <string>;

using namespace std;
UDP::UDP()
{
#ifdef WINDOWS
	WSAWrapper::instance();
#endif
	sockfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockfd == INVALID_SOCKET)
		throw runtime_error(format("'socket' error while opening UDP socket: {}", get_err_str()));
}

UDP::UDP(const Socket& self_socket) : UDP() { this->bind(self_socket); }

UDP::UDP(const PORT& self_port) : UDP(Socket{ {}, self_port }) {}

UDP::UDP(UDP&& other) noexcept
{
	this->sockfd = other.sockfd;
	other.sockfd = INVALID_SOCKET;
}

UDP& UDP::operator=(UDP&& other) noexcept
{
	if (this == &other)
		return *this;

	this->sockfd = other.sockfd;
	other.sockfd = INVALID_SOCKET;
	return *this;
}

io_task<void> UDP::bind(const Socket& self_socket)
{
	sockaddr_in serv_addr = self_socket;

	if (::bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		throw runtime_error(format("'bind' error while binding UDP socket on {}: {}", (string)self_socket, get_err_str()));
	co_return;
}

io_task<void> UDP::send(string_view sv, const Socket& remote) const
{
	sockaddr_in serv_addr = remote;
	int bytes_sent = sendto(sockfd, sv.data(), (int)sv.size(), 0, (sockaddr*)&serv_addr, sizeof(serv_addr));
	if (bytes_sent == SOCKET_ERROR)
		throw runtime_error(format("'sendto' error while sending '{}' to {}: {}", sv, (string)remote, get_err_str()));
	co_return;
}

io_task<pair<string, Socket>> UDP::receive() const
{
	sockaddr_in serv_addr{};
	socklen_t len = sizeof(serv_addr);
	string str(READ_MAX_SIZE, '\0');
	int n = recvfrom(sockfd, &str[0], READ_MAX_SIZE, 0, (sockaddr*)&serv_addr, &len);

	if (n == SOCKET_ERROR)
		throw runtime_error(format("'recvfrom' error while receiving data on PORT {}: {}", (string)this->get_self_port(), get_err_str()));

	str.resize(n);
	co_return { str, Socket{ serv_addr } };
}

PORT UDP::get_self_port() const
{
	sockaddr_in addr{};
	int len = sizeof(addr);
	if (::getsockname(sockfd, (sockaddr*)&addr, &len) == SOCKET_ERROR)
		throw runtime_error(format("'getsockname' error while fetching PORT info for UDP: {}", get_err_str()));
	return addr.sin_port;
}

io_task<void> UDP::close()
{
	if (sockfd != INVALID_SOCKET)
#ifdef WINDOWS
		::closesocket(sockfd);
#elif defined(UNIX)
		::close(sockfd);
#endif
	sockfd = INVALID_SOCKET;
	co_return;
}

UDP::~UDP()
{
	if (sockfd != INVALID_SOCKET)
#ifdef WINDOWS
		::closesocket(sockfd);
#elif defined(UNIX)
		::close(sockfd);
#endif
	sockfd = INVALID_SOCKET;
}