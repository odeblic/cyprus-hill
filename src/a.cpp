#include "Arguments.hpp"
#include "Buffer.hpp"
#include "Console.hpp"
#include "File.hpp"
#include "Network.hpp"

#include <thread>

auto console = Console("A");
auto client = ClientTCP{};

void processMessage(std::string const& content)
{
    try
    {
        console.outgoing(content);
        auto buffer = ByteBufferRO(content);
        client.getSocket().send(buffer);
        console.success("message forwarded");
    }
    catch(std::exception const& e)
    {
        console.exception(e);
    }
}

void readFromStdin()
{
    while (true)
    {
        auto line = console.prompt("enter a message:");
        console.incoming(line);
        processMessage(line);
    }
}

void readFromFile(std::string filename)
{
    auto file = File{filename};

    file.forEachLine([](std::string line)
    {
        console.incoming(line);
        processMessage(line);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // TODO: remove after debug
    });
}

void readFromNetwork(int port)
{
    auto server = ServerUDP{port};

    while (true)
    {
        auto buffer = ByteBufferRW<1024>();
        // TODO: handle the non-blocking case
        auto count = server.getSocket().receive(buffer);
        auto content = buffer.toString(count);
        console.incoming(content);
        processMessage(content);
    }
}

// TODO: handle signal INT to exit gracefully

int main(int argc, char ** argv)
try
{
    auto address = Address{"127.0.0.1", 5555}; // TODO: do not use hardcoded port for component B
    client.connect(address);
    console.success("connected to " + address.getString());

    if (argc == 1)
    {
        console.notice("will read messages from stdin");
        readFromStdin();
    }
    else if (argc == 2)
    {
        if (std::atoi(argv[1]) == 0)
        {
            auto const filename = std::string(argv[1]);
            console.notice("will read messages from file " + filename);
            readFromFile(filename);
        }
        else
        {
            auto const port = std::atoi(argv[1]);
            console.notice("will read messages from network on port " + std::to_string(port));
            readFromNetwork(port);
        }
    }
    else
    {
        // TODO: display usage
        throw ArgumentError("bad number of arguments");
    }

    console.notice("exiting...");
    return EXIT_SUCCESS;
}
catch (std::exception const& e)
{
    console.exception(e);
    return EXIT_FAILURE;
}
