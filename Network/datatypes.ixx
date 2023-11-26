module;
#include "network_import.h"

export module network.types;
import <iostream>;
import <vector>;
import <string>;
import <cstdint>;

export enum class ByteOrder
{
    NETWORK,
    HOST
};

export class IP
{
    std::uint32_t ip4_n{ 0 };
public:
    static std::vector<IP> get_local_ips();

    constexpr IP(std::uint32_t value = INADDR_ANY, ByteOrder order = ByteOrder::NETWORK);
    IP(std::string_view value);

    constexpr std::uint32_t get_ip(ByteOrder order = ByteOrder::NETWORK) const;
    operator std::string() const;

    constexpr bool operator==(const IP& other) const { return ip4_n == other.ip4_n; }
    constexpr bool operator!=(const IP& other) const { return ip4_n != other.ip4_n; }
    friend std::ostream& operator<< (std::ostream&, const IP&);
    friend std::istream& operator>> (std::istream&, IP&);
};

export class PORT
{
    std::uint16_t port_n{ 0 };
public:
    constexpr PORT(const uint16_t& value = 0, ByteOrder order = ByteOrder::NETWORK);

    constexpr uint16_t get_port(ByteOrder order = ByteOrder::NETWORK) const;
    constexpr operator uint16_t() const { return port_n; }

    operator std::string() const { return std::to_string(get_port(ByteOrder::HOST)); }
    constexpr bool operator==(const PORT& other) const { return port_n == other.port_n; }
    constexpr bool operator!=(const PORT& other) const { return port_n != other.port_n; }
    constexpr bool operator<(const PORT& other) const { return this->get_port(ByteOrder::HOST) < other.get_port(ByteOrder::HOST); }
    constexpr bool operator>(const PORT& other) const { return this->get_port(ByteOrder::HOST) != other.get_port(ByteOrder::HOST); }
    friend std::ostream& operator<< (std::ostream&, const PORT&);
    friend std::istream& operator>> (std::istream&, PORT&);
};

export class Socket
{
public:
    IP ip{};
    PORT port{};

    Socket(sockaddr_in& addr);
    Socket(IP ip = {}, PORT port = {});

    bool operator==(const Socket& other) const
    {
        return (ip == other.ip) && (port == other.port);
    }

    bool operator!=(const Socket& other) const
    {
        return (ip != other.ip) || (port != other.port);
    }

    operator sockaddr_in() const;
    operator std::string() const
    {
        return (std::string)ip + ":" + (std::string)port;
    }
    friend std::ostream& operator<< (std::ostream&, const Socket&);
};

export struct SocketPair
{
    Socket self{};
    Socket remote{};

    bool operator==(const SocketPair& other) const
    {
        return (self == other.self) && (remote == other.remote);
    }

    bool operator!=(const SocketPair& other) const
    {
        return (self != other.self) || (remote != other.remote);
    }

    operator std::string() const
    {
        return (std::string)self + "::" + (std::string)remote;
    }
    friend std::ostream& operator<< (std::ostream&, const SocketPair&);
};