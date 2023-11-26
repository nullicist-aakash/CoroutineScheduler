#pragma once

#include <string>

#define WINDOWS defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define UNIX defined(__unix__) || defined(__unix)

#if WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdexcept>
#include <system_error>

class WSAWrapper 
{
public:
    static WSADATA& instance()
    {
		static WSAWrapper sockInitializer;
		return sockInitializer.wsaData_;
	}

    ~WSAWrapper()
    {
        WSACleanup();
    }

private:
    WSAWrapper()
    {
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData_);
        if (result != 0)
            throw std::runtime_error("WSAStartup failed");
    }
    WSADATA wsaData_;
};

#elif UNIX
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#endif

consteval bool is_windows();
consteval bool is_unix();
std::string get_err_str();