module;
#include <stdexcept>
#include <format>
#include "network_import.h"
#include <system_error>

module network.types;
import <iostream>;
import <vector>;
import <string>;
import <cstdint>;

using namespace std;

std::string get_err_str()
{
    DWORD error = ::GetLastError();
    return std::system_category().message(error);
}

uint32_t get_ip_n(std::string_view value)
{
#ifdef WINDOWS
    struct sockaddr_in sa {};
    switch (InetPtonA(AF_INET, value.data(), &(sa.sin_addr)))
    {
    case 0:
        throw std::runtime_error(format("IP address '{}' doesn't belong to IPv4 family.", value));
    case -1:
        throw std::runtime_error(format("Invalid IP address '{}'. Error: {}", value, get_err_str()));
    }

    return (uint32_t)(sa.sin_addr.s_addr);
#elif UNIX
    uint32_t ip{};
    switch (inet_pton(AF_INET, value.data(), &ip))
    {
    case 0:
        throw std::runtime_error(format("IP address '{0}' doesn't belong to IPv4 family.", value));
    case -1:
        throw std::runtime_error(format("IP address '{0}' doesn't belong to any family.", value));
    }
    return ip;
#else
#error "Unknown platform"
#endif
}

std::vector<IP> IP::get_local_ips()
{
    std::vector<IP> ips;

#ifdef WINDOWS
    ULONG outBufLen = 0;
    DWORD dwRetVal = 0;

    // Make an initial call to GetAdaptersAddresses to get the required buffer size
    PIP_ADAPTER_ADDRESSES pAddresses = nullptr;
    if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &outBufLen) != ERROR_BUFFER_OVERFLOW) {
        std::cerr << "Error getting buffer size\n";
        return ips;
    }

    pAddresses = static_cast<IP_ADAPTER_ADDRESSES*>(malloc(outBufLen));
    if (pAddresses == nullptr) {
        std::cerr << "Memory allocation failed\n";
        return ips;
    }

    if (GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, pAddresses, &outBufLen) != NO_ERROR) {
        free(pAddresses);
        std::cerr << "GetAdaptersAddresses failed\n";
        return ips;
    }

    for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != nullptr; pCurrAddresses = pCurrAddresses->Next) {
        for (IP_ADAPTER_UNICAST_ADDRESS* pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast != nullptr; pUnicast = pUnicast->Next) {
            sockaddr_in* sockaddr_ipv4 = reinterpret_cast<sockaddr_in*>(pUnicast->Address.lpSockaddr);
            IP ip(ntohl(sockaddr_ipv4->sin_addr.s_addr));
            ips.push_back(ip);
        }
    }

    if (pAddresses != nullptr) {
        free(pAddresses);
    }
#elif UNIX
    struct ifaddrs* ifAddrStruct = nullptr;
    if (getifaddrs(&ifAddrStruct) != 0) {
        return ips;
    }

    for (struct ifaddrs* ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET)
            continue;

        auto ipn = ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr.s_addr;
        if (ipn == 0 || ipn == 0xFFFFFFFF)
            continue;

        ips.push_back(ipn);
    }

    freeifaddrs(ifAddrStruct);
#else
#error "Unknown platform"
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

Socket::Socket(socket_addr& addr) : ip{ addr.sin_addr.s_addr }, port{ addr.sin_port } { }

Socket::Socket(IP ip, PORT port) : ip{ ip }, port{ port } { }

Socket::operator socket_addr() const
{
    socket_addr addr;
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