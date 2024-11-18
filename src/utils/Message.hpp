#pragma once

#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

struct MessageError : std::runtime_error
{
    MessageError(std::string const& message)
    : std::runtime_error(message)
    {
    }
};

/*
struct CreationError : SocketError
{
    CreationError()
    : SocketError("Socket creation failed: " + std::string(strerror(errno)))
    {
    }
};
*/

struct Message
{
    struct Entry
    {
        unsigned char price;
        unsigned char volume;
    };

    std::string stringify() const
    {
        throw MessageError("not implemented");
    }

    static Message parse(std::string const& line)
    {
        if (line.empty())
        {
            throw MessageError("bad format! (empty string)");
        }

        size_t const size = (*line.rbegin() == '\n') ? line.size() - 1 : line.size();

        if (size % 2)
        {
            throw MessageError("bad format! (odd number of characters)");
        }

        auto message = Message{};

        for (auto index = 0; index < size; index++)
        {
            auto const price = static_cast<unsigned char>(line[index]);
            auto const volume = static_cast<unsigned char>(line[++index]);
            auto entry = Message::Entry{price, volume};
            message.entries.push_back(entry);
        }

        return message;
    }

    std::string serialize() const
    {
        throw MessageError("not implemented");
    }

    static Message deserialize(std::string const& buffer)
    {
        throw MessageError("not implemented");
    }

    std::string line;
    std::vector<Entry> entries;
};
