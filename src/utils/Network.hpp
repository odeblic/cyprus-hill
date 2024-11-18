#pragma once

#include "Address.hpp"
#include "Errors.hpp"
#include "Socket.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <utility>

template <typename SocketType>
class Endpoint
{
public:
    Endpoint()
    {
    }

    Endpoint(Port port)
    : localAddr(port)
    {
    }

    Endpoint(Host host, Port port)
    : localAddr(host, port)
    {
    }

    SocketType getSocket()
    {
        return sock;
    }

protected:
    Address localAddr;
    SocketType sock;
};

template <typename SocketType>
class Server : public Endpoint<SocketType>
{
public:
    template<typename... Args>
    Server(Args&&... args)
    : Endpoint<SocketType>(std::forward<Args>(args)...)
    {
        if (::bind(this->sock.getFD(), this->localAddr.getBytes(), *this->localAddr.getLength()) < 0)
        {
            throw BindError();
        }
    }
};

class ServerTCP : public Server<SocketTCP>
{
public:
    template<typename... Args>
    ServerTCP(Args&&... args)
    : Server<SocketTCP>(std::forward<Args>(args)...)
    {
        if (::listen(sock.getFD(), 5) < 0)
        {
            throw ListenError();
        }
    }

    SocketTCP accept(Address& remoteAddr)
    {
        int fd = ::accept(sock.getFD(), remoteAddr.getBytes(), remoteAddr.getLength());

        if (fd < 0)
        {
            throw AcceptError();
        }

        return SocketTCP(fd);
    }
};

class ServerUDP : public Server<SocketUDP>
{
public:
    template<typename... Args>
    ServerUDP(Args&&... args)
    : Server<SocketUDP>(std::forward<Args>(args)...)
    {
    }
};

class ClientTCP : public Endpoint<SocketTCP>
{
public:
    ClientTCP()
    {
    }

    template<typename... Args>
    ClientTCP(Args&&... args)
    : Endpoint<SocketTCP>(std::forward<Args>(args)...)
    {
    }

    void connect(Address remoteAddr)
    {
        if (::connect(sock.getFD(), remoteAddr.getBytes(), *remoteAddr.getLength()) < 0)
        {
            throw ConnectionError();
        }
    }
};

class ClientUDP : public Endpoint<SocketUDP>
{
public:
    ClientUDP()
    {
    }

    template<typename... Args>
    ClientUDP(Args&&... args)
    : Endpoint<SocketUDP>(std::forward<Args>(args)...)
    {
    }
};
