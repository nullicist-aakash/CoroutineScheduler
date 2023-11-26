module;
#include <stdexcept>
#include <format>
#include "network_import.h"

module network.types;
import <iostream>;
import <vector>;
import <string>;
import <cstdint>;

using namespace std;

uint32_t get_ip_n(std::string_view value)
{
    uint32_t ip{};
#if WINDOWS
    switch (InetPtonA(AF_INET, value.data(), &ip))
#elif UNIX
    switch (inet_pton(AF_INET, value.data(), &ip))
#endif
    {
    case 0:
        throw std::runtime_error(format("IP address '{}' doesn't belong to IPv4 family.", value));
    case -1:
        throw std::runtime_error(format("Invalid IP address '{}'. Error: {}", value, get_err_str()));
    }

    return ip;
}

std::vector<IP> IP::get_local_ips()
{
    std::vector<IP> ips;

#if WINDOWS
    ULONG outBufLen = 0;
    DWORD dwRetVal = 0;

    if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &outBufLen) != ERROR_BUFFER_OVERFLOW)
        throw std::runtime_error("'GetAdaptersAddresses': Error getting buffer size");

    PIP_ADAPTER_ADDRESSES pAddresses = static_cast<IP_ADAPTER_ADDRESSES*>(malloc(outBufLen));
    if (pAddresses == nullptr)
        throw std::runtime_error("'GetAdaptersAddresses': Error getting buffer size");

    if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses, &outBufLen) != NO_ERROR)
    {
        free(pAddresses);
        throw std::runtime_error("GetAdaptersAddresses failed");
    }

    for (auto cur = pAddresses; cur; cur = cur->Next)
    {
        for (auto* pUnicast = cur->FirstUnicastAddress; pUnicast; pUnicast = pUnicast->Next) 
        {
            auto sockaddr_ipv4 = (sockaddr_in*)pUnicast->Address.lpSockaddr;
            ips.push_back(sockaddr_ipv4->sin_addr.s_addr);
        }
    }

    if (pAddresses != nullptr)
        free(pAddresses);

#elif UNIX
    struct ifaddrs* ifAddrStruct = nullptr;
    if (getifaddrs(&ifAddrStruct) != 0)
        return ips;

    for (auto ifa = ifAddrStruct; ifa; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET)
            continue;

        auto ipn = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr;
        if (ipn == 0 || ipn == 0xFFFFFFFF)
            continue;

        ips.push_back(ipn);
    }

    freeifaddrs(ifAddrStruct);
#endif

    return ips;
}

IP::IP(std::string_view value) : ip4_n{ get_ip_n(value) }
{

}

constexpr IP::IP(uint32_t value, ByteOrder order) : ip4_n
{
    order == ByteOrder::NETWORK ? value : htonl(value)
} { }

constexpr uint32_t IP::get_ip(ByteOrder order) const
{
    return order == ByteOrder::NETWORK ? ip4_n : ntohl(ip4_n);
}

IP::operator std::string() const
{
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip4_n, str, INET_ADDRSTRLEN);
    return std::string(str);
}

std::ostream& operator<< (std::ostream& out, const IP& ip)
{
    out << (std::string)ip;
    return out;
}

std::istream& operator>> (std::istream& in, IP& ip)
{
    std::string str;
    in >> str;
    ip = IP(str);
    return in;
}

constexpr PORT::PORT(const uint16_t& value, ByteOrder order) : port_n
{
    order == ByteOrder::NETWORK ? value : htons(value)
} { }

constexpr uint16_t PORT::get_port(ByteOrder order) const
{
    return order == ByteOrder::NETWORK ? port_n : ntohs(port_n);
}

std::ostream& operator<< (std::ostream& out, const PORT& port)
{
    out << port.get_port(ByteOrder::HOST);
    return out;
}

std::istream& operator>> (std::istream& in, PORT& port)
{
    uint16_t prt;
    in >> prt;
    port = PORT(prt, ByteOrder::HOST);
    return in;
}

Socket::Socket(sockaddr_in& addr) : ip{ addr.sin_addr.s_addr }, port{ addr.sin_port } { }

Socket::Socket(IP ip, PORT port) : ip{ ip }, port{ port } { }

Socket::operator sockaddr_in() const
{
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ip.get_ip();
    addr.sin_port = port.get_port();
    return addr;
}

std::ostream& operator<< (std::ostream& out, const Socket& socket)
{
    out << (string)socket;
    return out;
}

std::ostream& operator<< (std::ostream& out, const SocketPair& sp)
{
    out << (string)sp;
    return out;
}