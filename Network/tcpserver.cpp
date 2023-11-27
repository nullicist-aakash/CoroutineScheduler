module;
#include <format>
#include "network_import.h"

module network.tcp.server;
import network.types;
import <utility>;
import <string>;

using namespace std;
constexpr auto LISTEN_LEN = 0x7fff;

TCPServer::TCPServer(const Socket& self_socket) : self_socket(self_socket)
{
#ifdef WINDOWS
	WSAWrapper::instance();
#endif
	listenfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenfd == INVALID_SOCKET)
		throw runtime_error(format("'socket' error while creating TCP Server socket {} : {}", (string)self_socket, get_err_str()));

	struct sockaddr_in serv_addr = self_socket;

	if (::bind(listenfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
		throw runtime_error(format("'bind' error while creating TCP Server socket {} : {}", (string)self_socket, get_err_str()));

	if (::listen(listenfd, LISTEN_LEN) == SOCKET_ERROR)
		throw runtime_error(format("'listen' error while creating TCP Server socket {} : {}", (string)self_socket, get_err_str()));
}

TCPServer::TCPServer(TCPServer&& other) : self_socket(other.self_socket)
{
	this->listenfd = other.listenfd;
	other.listenfd = INVALID_SOCKET;
	other.self_socket = {};
}

TCPServer& TCPServer::operator=(TCPServer&& other)
{
	if (&other == this)
		return *this;

	this->listenfd = other.listenfd;
	this->self_socket = other.self_socket;
	other.listenfd = INVALID_SOCKET;
	other.self_socket = {};
	return *this;
}

const Socket& TCPServer::get_socket() const
{
	return self_socket;
}

TCP TCPServer::accept() const
{
	sockaddr_in cliaddr{};
	socklen_t clilen = sizeof(cliaddr);

	auto sockfd = ::accept(listenfd, (sockaddr*)&cliaddr, &clilen);

	if (sockfd == INVALID_SOCKET)
		throw runtime_error(format("'accept' error on TCP server {}: {}", (string)self_socket, get_err_str()));

	struct sockaddr_in self_addr {};
	socklen_t len = sizeof(self_addr);
	if (getsockname(sockfd, (struct sockaddr*)&self_addr, &len) == SOCKET_ERROR)
		throw runtime_error(
			format("'getsockname' error after accepting connection on TCP server {}: {}", (string)self_socket, get_err_str()));

	return TCP(sockfd, SocketPair{ self_addr, cliaddr });
}

void TCPServer::close()
{
	if (listenfd != INVALID_SOCKET)
#ifdef WINDOWS
		::closesocket(listenfd);
#elif defined(UNIX)
	::close(listenfd);
#endif
	listenfd = INVALID_SOCKET;
}

TCPServer::~TCPServer()
{
	this->close();
}