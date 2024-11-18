#include "Console.hpp"
#include "Network.hpp"

#include <cstdlib>
#include <string>

auto console = Console("S");

// TODO: handle signal INT to exit gracefully

int main(int argc, char ** argv)
try
{
    if (argc != 2)
    {
        // TODO: display usage
        console.error("bad arguments count");
    }

    int const port = std::atoi(argv[1]);

    if (port == 0)
    {
        console.error("invalid port number");
    }

    auto server = ServerTCP{port};
    console.success("listening on port " + std::to_string(port));

    auto inputBuffer = ByteBufferRW<1024>();
    auto outputBuffer = ByteBufferRO("OK\n");

    while (true)
    {
        console.notice("waiting for component B to connect...");
        Address remoteAddr;
        auto socket = server.accept(remoteAddr);
        console.notice("connection from " + remoteAddr.getString());

        while (true)
        {
            try
            {
                auto const count = socket.receive(inputBuffer);

                if (count > 0)
                {
                    auto const text = inputBuffer.toString(count);
                    console.notice("received from " + remoteAddr.getString());
                    console.incoming(text);

                    if (count == 5)
                    {
                        console.success("valid size: " + std::to_string(count));
                        console.outgoing(outputBuffer.toString());
                        socket.send(outputBuffer);
                    }
                    else
                    {
                        console.failure("invalid size: " + std::to_string(count));
                    }
                }
            }
            catch (PeerError const& e)
            {
                console.notice("component B has disconnected");
                break;
            }
        }
    }

    console.notice("exiting...");
    return EXIT_SUCCESS;
}
catch (std::exception const& e)
{
    console.exception(e);
    return EXIT_FAILURE;
}
