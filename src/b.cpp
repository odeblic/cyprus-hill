#include "Arguments.hpp"
#include "Buffer.hpp"
#include "Console.hpp"
#include "Message.hpp"
#include "Network.hpp"

#include <atomic>
#include <condition_variable>
#include <queue>
#include <thread>

auto console = Console("B");
auto server = ServerTCP{};
auto client = ClientTCP{};

std::atomic<bool> running{true};
std::queue<std::string> messageQueue;
std::mutex mutex;
std::condition_variable cv;

void handleMessagesFromComponentA()
{
    while (running)
    {
        console.notice("waiting for component A to connect...");
        Address remoteAddr;
        auto socket = server.accept(remoteAddr);
        socket.setNonBlockingMode();
        console.success("connection from " + remoteAddr.getString());

        while (running)
        {
            auto inputBuffer = ByteBufferRW<1024>();

            try
            {
                auto const count = socket.receive(inputBuffer);

                if (count > 0)
                {
                    auto const content = inputBuffer.toString(count);
                    console.incoming(content);
                    std::unique_lock<std::mutex> lock(mutex);
                    messageQueue.push(content);
                    cv.notify_one();
                }
            }
            catch (PeerError const& e)
            {
                console.notice("component A has disconnected");
                break;
            }
        }
    }
}

void sendToExternalServer()
{
    while (running)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, []{ return not messageQueue.empty() or not running; });

        while (not messageQueue.empty())
        {
            std::string message = messageQueue.front();
            messageQueue.pop();
            lock.unlock();

            for (auto const& entry : Message::parse(message).entries)
            {
                auto const price = static_cast<char>(entry.price);

                if (price > 80)
                {
                    console.notice("price greater than 80");
                    auto response = std::string(5, price);
                    console.outgoing(response);
                    auto outputBuffer = ByteBufferRO(response);
                    client.getSocket().send(outputBuffer);
                    console.success("message sent to external server");
                }
            }

            lock.lock();
        }
    }
}

void receiveFromExternalServer()
{
    auto inputBuffer = ByteBufferRW<1024>();
    auto socket = client.getSocket();
    socket.setNonBlockingMode();

    while (running)
    {
        try
        {
            auto const count = socket.receive(inputBuffer);

            if (count > 0)
            {
                auto const text = inputBuffer.toString(count);
                console.incoming(text);
            }
        }
        catch (PeerError const& e)
        {
            console.notice("peer (external server) has disconnected");
            running = false;
            break;
        }
    }
}

// TODO: handle signal INT to exit gracefully

int main(int argc, char ** argv)
try
{
    // TODO: accept a different host than localhost

    if (argc == 3)
    {
        Port const listeningPort = std::atoi(argv[1]);
        Port const connectingPort = std::atoi(argv[2]);

        if (listeningPort == 0)
        {
            throw ArgumentError("listening port is invalid");
        }

        if (connectingPort == 0)
        {
            throw ArgumentError("connecting port is invalid");
        }

        server = ServerTCP{listeningPort};
        console.notice("listening on port " + std::to_string(listeningPort));

        auto address = Address{"127.0.0.1", connectingPort};
        client.connect(address);
        console.success("connected to port " + std::to_string(connectingPort));
    }
    else
    {
        // TODO: display usage
        throw ArgumentError("bad number of arguments");
    }

    auto thread1 = std::thread(handleMessagesFromComponentA);
    auto thread2 = std::thread(sendToExternalServer);
    auto thread3 = std::thread(receiveFromExternalServer);

    thread1.join();
    thread2.join();
    thread3.join();

    console.notice("exiting...");
    return EXIT_SUCCESS;
}
catch (std::exception const& e)
{
    console.exception(e);
    return EXIT_FAILURE;
}
