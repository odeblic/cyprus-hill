#pragma once

#include "Errors.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

typedef std::string Host;

class Port
{
public:
    Port(int number)
    {
        if (0x0001 <= number and number <= 0xFFFF)
        {
            this->port = number;
        }
        else
        {
            throw PortError();
        }
    }

    Port(std::string string)
    : Port(std::stoi(string))
    {
    }

    operator in_port_t() const
    {
        return port;
    }

private:
    in_port_t port{0};
};

class Address
{
public:
    Address()
    {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
    }

    Address(Port port)
    : Address()
    {
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;
    }

    Address(Host host, Port port)
    : Address(port)
    {
        if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0)
        {
            throw AddressError(host);
        }
    }

    Port getPort() const
    {
        return ntohs(addr.sin_port);
    }

    std::string getHost() const
    {
        char buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, buffer, sizeof(buffer));
        return std::string(buffer);
    }

    std::string getString() const
    {
        return getHost() + ":" + std::to_string(getPort());
    }

    // TODO: make getters callable on const objects

    struct sockaddr * getBytes()
    {
        return reinterpret_cast<struct sockaddr*>(&addr);
    }

    socklen_t * getLength()
    {
        return &len;
    }

private:
    struct sockaddr_in addr{};
    socklen_t len{sizeof(addr)};
};
