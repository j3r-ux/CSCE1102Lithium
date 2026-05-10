#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

enum class MessageType {
    Invalid,
    Login,
    Join,
    Leave,
    RoomMsg,
    PrivateMsg
};

// One flat struct for every kind of message. Unused fields stay empty / zero.
struct Message {
    MessageType type = MessageType::Invalid;
    std::string username;   // LOGIN, JOIN (optional)   
    std::string sender;     // ROOM_MSG, PRIVATE_MSG
    std::string target;     // PRIVATE_MSG
    std::string text;       // ROOM_MSG, PRIVATE_MSG
    std::string roomIdStr;  // JOIN, LEAVE (sent as string by Qt)
    int         roomId = 0; // ROOM_MSG (sent as integer)
};

// Parses one line of JSON. Returns true on success, fills `msg`.
// On failure returns false, leaves msg.type = Invalid, fills `error`.
bool parseMessage(const std::string& json, Message& msg, std::string& error);

// Server-side serializers. Each produces one line of compact JSON, no '\n'.
std::string roomMsgToJson(const Message& msg);
std::string privateMsgToJson(const Message& msg);
std::string serverError(const std::string& text);
std::string serverNotice(const std::string& text);

#endif
