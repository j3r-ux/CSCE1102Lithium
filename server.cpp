#include "protocol.h"
#include "chatstate.h"

#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>

#include <iostream>
#include <istream>
#include <string>
#include <unordered_map>

namespace asio = boost::asio;
using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;

class Hub{
public:
    int registerSocket(tcp::socket* socket)
    {
        const int id = nextId_++;
        sockets_[id] = socket;
        return id;
    }

    void unregisterSocket(int id)
    {
        sockets_.erase(id);
        state.detach(id);
    }

    // Append '\n' and write the line. Sync write is fine: messages are short and there's nothing else for this thread to do during the write anyway.
    void deliver(int id, const std::string& line)
    {
        auto it = sockets_.find(id);
        if (it == sockets_.end()) return; // client already disconnected, drop the message
        boost::system::error_code ec;
        const std::string framed = line + "\n"; // protocol is line-delimited, append the terminator
        asio::write(*it->second, asio::buffer(framed), ec);
    }

    ChatState state;

private:
    std::unordered_map<int, tcp::socket*> sockets_;
    int nextId_ = 1;
};

// ---------- Dispatch: one parsed message -> chat-state update + sends ---------

static void dispatch(const Message& msg, int origin, Hub& hub)
{
    switch (msg.type) {
    case MessageType::Login: {
        if (!hub.state.attach(origin, msg.username)) {
            hub.deliver(origin, serverError("username '" + msg.username + "' is taken"));
        } else {
            hub.deliver(origin, serverNotice("logged in as " + msg.username));
        }
        break;
    }

    case MessageType::Join: {
        int roomId = 0;
        try { roomId = std::stoi(msg.roomIdStr); } // Qt sends roomId as a string for JOIN/LEAVE
        catch (const std::exception&) {
            hub.deliver(origin, serverError("JOIN: roomId must be numeric"));
            return;
        }
        // client may JOIN before LOGIN — auto-attach using the username on the JOIN
        if (!hub.state.isAttached(origin) && !msg.username.empty()) {
            if (!hub.state.attach(origin, msg.username)) {
                hub.deliver(origin, serverError("username '" + msg.username + "' is taken"));
                return;
            }
        }
        hub.state.joinRoom(origin, roomId);
        hub.deliver(origin, serverNotice("joined room " + std::to_string(roomId)));
        break;
    }

    case MessageType::Leave: {
        int roomId = 0;
        try { roomId = std::stoi(msg.roomIdStr); }
        catch (const std::exception&) {
            hub.deliver(origin, serverError("LEAVE: roomId must be numeric"));
            return;
        }
        hub.state.leaveRoom(origin, roomId);
        break;
    }

    case MessageType::RoomMsg: {
        if (!hub.state.isAttached(origin)) {
            hub.state.attach(origin, msg.sender); // auto-attach using the sender field
        }
        // Sender is implicitly in their own room.
        hub.state.joinRoom(origin, msg.roomId);

        const std::string line = roomMsgToJson(msg);
        // fan out to every member of the room except the sender
        for (int sid : hub.state.roomMembers(msg.roomId, true, origin)) {
            hub.deliver(sid, line);
        }
        break;
    }

    case MessageType::PrivateMsg: {
        if (!hub.state.isAttached(origin)) {
            hub.state.attach(origin, msg.sender); // auto-attach using the sender field
        }
        int targetId = 0;
        if (!hub.state.sessionOf(msg.target, targetId)) {
            hub.deliver(origin, serverError("user '" + msg.target + "' is not online"));
            return;
        }
        hub.deliver(targetId, privateMsgToJson(msg)); // DM goes only to the target session
        break;
    }

    case MessageType::Invalid:
    default:
        break;
    }
}

// ---------- One client: a coroutine that reads lines until disconnect ---------

static awaitable<void> handleClient(tcp::socket socket, Hub& hub)
{
    // register: assign this connection an id and put it in the socket map
    const int id = hub.registerSocket(&socket);
    asio::streambuf buffer;

    // read loop: pull one line at a time, parse, dispatch
    try {
        while (true) {
            co_await asio::async_read_until(socket, buffer, '\n', use_awaitable);

            std::istream input(&buffer);
            std::string line;
            std::getline(input, line);
            if (line.empty()) continue; // blank line, skip

            Message msg;
            std::string err;
            if (!parseMessage(line, msg, err)) {
                hub.deliver(id, serverError(err)); // bad JSON: tell the client and keep reading
                continue;
            }
            dispatch(msg, id, hub);
        }
    } catch (const std::exception&) {
        // Read failed — usually the peer closed the connection.
    }

    // disconnect: log who left, then clean up state and the socket map
    std::string name;
    if (hub.state.usernameOf(id, name)) {
        std::cout << "client " << id << " (" << name << ") disconnected\n";
    } else {
        std::cout << "client " << id << " disconnected\n";
    }
    hub.unregisterSocket(id);
}

// ---------- Listener: accept loop, also a coroutine --------------------------

static awaitable<void> listener(asio::io_context& io, Hub& hub)
{
    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 54321));
    std::cout << "Server listening on 127.0.0.1:54321" << std::endl;

    // accept loop: every new connection gets its own coroutine
    while (true) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(io, handleClient(std::move(socket), hub), detached);
    }
}

// ---------- Main: one thread, one io_context --------------------------------

int main()
{
    try {
        asio::io_context io;
        Hub hub;
        co_spawn(io, listener(io, hub), detached);
        io.run();
    } catch (const std::exception& e) {
        std::cerr << "fatal: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
