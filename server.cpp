#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

awaitable<void> handle_client(tcp::socket socket) {
    boost::asio::streambuf buffer;

    while (true) {
        auto [ec, bytes_read] = co_await boost::asio::async_read_until(
            socket,
            buffer,
            '\n',
            as_tuple(use_awaitable)
        );

        if (ec) {
            std::cout << "Client disconnected.\n";
            break;
        }

        std::istream stream(&buffer);
        std::string line;
        std::getline(stream, line);

        try {
            boost::json::value parsed = boost::json::parse(line);
            boost::json::object obj = parsed.as_object();

            std::string type = std::string(obj["type"].as_string());

            std::cout << "\n--- Message Received ---\n";
            std::cout << "Type: " << type << "\n";

            if (type == "JOIN") {
                std::cout << "Room ID: " << obj["roomId"].as_string() << "\n";
            }
            else if (type == "ROOM_MSG") {
                std::cout << "Sender: " << obj["sender"].as_string() << "\n";
                std::cout << "Room ID: " << obj["roomId"] << "\n";
                std::cout << "Text: " << obj["text"].as_string() << "\n";
            }
            else if (type == "PRIVATE_MSG") {
                std::cout << "Sender: " << obj["sender"].as_string() << "\n";
                std::cout << "Target: " << obj["targetUser"].as_string() << "\n";
                std::cout << "Text: " << obj["text"].as_string() << "\n";
            }
            std::cout << "------------------------\n";

        } catch (...) {
            std::cout << "Invalid JSON received.\n";
        }
    }
}

awaitable<void> listener() {
    auto io_ctx = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(io_ctx, { tcp::v4(), 54321 });

    std::cout << "Server is listening on 127.0.0.1: port 54321...\n";

    while (true) {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        if (!ec) {
            co_spawn(io_ctx, handle_client(std::move(socket)), detached);
        }
        else {
            std::cout << "Accept error: " << ec.message() << "\n";
        }
    }
}

int main() {
    boost::asio::io_context io_context;
    co_spawn(io_context, listener(), detached);
    io_context.run();
    return 0;
}
