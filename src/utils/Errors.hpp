#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

// TODO: embed the errno in the exception

struct SocketError : std::runtime_error
{
    SocketError(std::string const& message)
    : std::runtime_error(message)
    {
    }
};

struct CreationError : SocketError
{
    CreationError()
    : SocketError("Socket creation failed: " + std::string(strerror(errno)))
    {
    }
};

struct AddressError : SocketError
{
    AddressError(std::string host)
    : SocketError("Invalid address: " + host)
    {
    }
};

// TODO: improve the message and give more error context
struct PortError : SocketError
{
    PortError()
    : SocketError("Invalid port")
    {
    }
};

struct ModeError : SocketError
{
    ModeError()
    : SocketError("Socket mode get/set failed: " + std::string(strerror(errno)))
    {
    }
};

struct BindError : SocketError
{
    BindError()
    : SocketError("Socket bind failed: " + std::string(strerror(errno)))
    {
    }
};

struct ListenError : SocketError
{
    ListenError()
    : SocketError("Socket listen failed: " + std::string(strerror(errno)))
    {
    }
};

struct AcceptError : SocketError
{
    AcceptError()
    : SocketError("Socket accept failed: " + std::string(strerror(errno)))
    {
    }
};

struct ConnectionError : SocketError
{
    ConnectionError()
    : SocketError("Socket connect failed: " + std::string(strerror(errno)))
    {
    }
};

struct SendError : SocketError
{
    SendError()
    : SocketError("Socket send failed: " + std::string(strerror(errno)))
    {
    }
};

struct ReceiveError : SocketError
{
    ReceiveError()
    : SocketError("Socket receive failed: " + std::string(strerror(errno)))
    {
    }
};

struct PeerError : SocketError
{
    PeerError()
    : SocketError("Socket shutdown by peer: ")
    {
    }
};

struct CloseError : SocketError
{
    CloseError()
    : SocketError("Socket close failed: " + std::string(strerror(errno)))
    {
    }
};

struct ShutdownError : SocketError
{
    ShutdownError()
    : SocketError("Socket shutdown failed: " + std::string(strerror(errno)))
    {
    }
};
