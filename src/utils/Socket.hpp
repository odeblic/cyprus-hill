#pragma once

#include "Address.hpp"
#include "Buffer.hpp"
#include "Errors.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

class Socket
{
public:
    class FileDescriptor
    {
    public:
        FileDescriptor(int fd)
        : fd(fd)
        {
            if (fd < 0)
            {
                throw CreationError();
            }
        }

        ~FileDescriptor()
        {
            ::close(fd);
        }

        int get() const
        {
            return fd;
        }

    private:
        int fd{-1};
    };

    enum class ShutDownMethod
    {
        RD = ::SHUT_RD,
        WR = ::SHUT_WR,
        RDWR = ::SHUT_RDWR,
    };

    int getFD() const
    {
        return fd->get();
    }

    void setNonBlockingMode()
    {
        int const flags = ::fcntl(getFD(), F_GETFL, 0);

        if (flags == -1)
        {
            throw ModeError();
        }

        if (::fcntl(getFD(), F_SETFL, flags | O_NONBLOCK) == -1)
        {
            throw ModeError();
        }
    }

    template <typename BufferType>
    size_t send(BufferType const& buffer)
    {
        ssize_t const count = ::send(getFD(), buffer.getData(), buffer.getSize(), 0);

        if (count < 0)
        {
            throw SendError();
        }

        return count;
    }

    template <typename BufferType>
    size_t receive(BufferType& buffer)
    {
        ssize_t const count = ::recv(getFD(), buffer.getData(), buffer.getSize(), 0);

        if (count < 0)
        {
            if (errno == EAGAIN)
            {
                return 0;
            }
            else
            {
                throw ReceiveError();
            }
        }
        else if (count == 0)
        {
            throw PeerError();
        }
        else
        {
            return count;
        }
    }

    void close()
    {
        // TODO: reset the shared pointer

        if (::close(getFD()) < 0)
        {
            throw CloseError();
        }
    }

    void shutdown(ShutDownMethod method)
    {
        if (::shutdown(getFD(), static_cast<int>(method)) < 0)
        {
            throw ShutdownError();
        }
    }

protected:
    Socket(int fd)
    : fd(std::make_shared<FileDescriptor>(fd))
    {
    }

private:
    std::shared_ptr<FileDescriptor> fd;
};

class SocketTCP : public Socket
{
public:
    SocketTCP()
    : SocketTCP(::socket(AF_INET, SOCK_STREAM, 0))
    {
    }

    SocketTCP(int fd)
    : Socket(fd)
    {
    }
};

class SocketUDP : public Socket
{
public:
    SocketUDP()
    : SocketUDP(::socket(AF_INET, SOCK_DGRAM, 0))
    {
    }

    SocketUDP(int fd)
    : Socket(fd)
    {
    }

    template <typename BufferType>
    size_t sendTo(BufferType const& buffer, Address addr)
    {
        ssize_t const count = ::sendto(getFD(), buffer.getData(), buffer.getSize(), 0, addr.getBytes(), *addr.getLength());

        if (count < 0)
        {
            throw SendError();
        }

        return count;
    }

    template <typename BufferType>
    size_t receiveFrom(BufferType& buffer, Address& addr)
    {
        ssize_t const count = ::recvfrom(getFD(), buffer.getData(), buffer.getSize(), 0, addr.getBytes(), addr.getLength());

        if (count < 0)
        {
            if (errno == EAGAIN)
            {
                return 0;
            }
            else
            {
                throw ReceiveError();
            }
        }
        else if (count == 0)
        {
            throw PeerError();
        }
        else
        {
            return count;
        }
    }
};
