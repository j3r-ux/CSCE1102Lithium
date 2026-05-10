#include "protocol.h"

#include <cctype>
#include <stdexcept>

using namespace std;

/* 
We don't need a full JSON parser. every message we send is one flat object with string and integer fields, 
and we control both ends. So just look for "key":<value> directly. 
The escape handling covers the characters Qt's compact JSON ever emits in our text fields.
*/

static bool findString(const string& json, const string& key, string& out)
{
    // locate "key": in the json
    const string needle = "\"" + key + "\":";
    size_t p = json.find(needle);
    if (p == string::npos) return false;

    // skip past the key, then any whitespace, then expect an opening quote
    p += needle.size();
    while (p < json.size() && isspace(static_cast<unsigned char>(json[p]))) ++p;
    if (p >= json.size() || json[p] != '"') return false;
    ++p; // step past the opening quote

    // walk the value char by char, decoding escape sequences as we go
    out.clear();
    while (p < json.size() && json[p] != '"') {
        if (json[p] == '\\' && p + 1 < json.size()) {
            char e = json[p + 1];
            switch (e) {
                case 'n':  out.push_back('\n'); break;
                case 't':  out.push_back('\t'); break;
                case 'r':  out.push_back('\r'); break;
                case '"':  out.push_back('"');  break;
                case '\\': out.push_back('\\'); break;
                case '/':  out.push_back('/');  break;
                default:   out.push_back(e);    break;
            }
            p += 2;
        } else {
            out.push_back(json[p++]);
        }
    }
    return p < json.size(); // true only if we reached the closing quote
}

static bool findInt(const string& json, const string& key, int& out)
{
    // locate "key": in the json
    const string needle = "\"" + key + "\":";
    size_t p = json.find(needle);
    if (p == string::npos) return false;

    // skip past the key, then any whitespace
    p += needle.size();
    while (p < json.size() && isspace(static_cast<unsigned char>(json[p]))) ++p;

    // capture optional minus sign + digit run
    size_t start = p;
    if (p < json.size() && json[p] == '-') ++p;
    size_t digitsStart = p;
    while (p < json.size() && isdigit(static_cast<unsigned char>(json[p]))) ++p;
    if (p == digitsStart) return false; // no digits found

    try {
        out = stoi(json.substr(start, p - start));
    } catch (const exception&) {
        return false; // overflow or other stoi failure
    }
    return true;
}

static void appendEscaped(string& out, const string& s)
{
    // wrap s in quotes, escaping the chars JSON requires
    out.push_back('"');
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:   out.push_back(c);
        }
    }
    out.push_back('"');
}

// ---------- Public API --------------------------------------------------------

bool parseMessage(const string& json, Message& msg, string& error)
{
    msg = Message();
    error.clear();

    string type;
    if (!findString(json, "type", type)) {
        error = "missing 'type' field";
        return false;
    }

    if (type == "LOGIN") {
        if (!findString(json, "username", msg.username)) {
            error = "LOGIN: missing 'username'";
            return false;
        }
        msg.type = MessageType::Login;
        return true;
    }
    if (type == "JOIN") {
        if (!findString(json, "roomId", msg.roomIdStr)) {
            error = "JOIN: missing 'roomId'";
            return false;
        }
        findString(json, "username", msg.username); // optional
        msg.type = MessageType::Join;
        return true;
    }
    if (type == "LEAVE") {
        if (!findString(json, "roomId", msg.roomIdStr)) {
            error = "LEAVE: missing 'roomId'";
            return false;
        }
        msg.type = MessageType::Leave;
        return true;
    }
    if (type == "ROOM_MSG") {
        if (!findString(json, "sender", msg.sender)) { error = "ROOM_MSG: missing 'sender'"; return false; }
        if (!findInt   (json, "roomId", msg.roomId)) { error = "ROOM_MSG: missing/invalid 'roomId'"; return false; }
        if (!findString(json, "text",   msg.text))   { error = "ROOM_MSG: missing 'text'"; return false; }
        if (msg.text.empty())                         { error = "ROOM_MSG: empty text"; return false; }
        msg.type = MessageType::RoomMsg;
        return true;
    }
    if (type == "PRIVATE_MSG") {
        if (!findString(json, "sender",     msg.sender)) { error = "PRIVATE_MSG: missing 'sender'"; return false; }
        if (!findString(json, "targetUser", msg.target)) { error = "PRIVATE_MSG: missing 'targetUser'"; return false; }
        if (!findString(json, "text",       msg.text))   { error = "PRIVATE_MSG: missing 'text'"; return false; }
        if (msg.text.empty())                             { error = "PRIVATE_MSG: empty text"; return false; }
        msg.type = MessageType::PrivateMsg;
        return true;
    }

    error = "unknown message type: " + type;
    return false;
}

string roomMsgToJson(const Message& m)
{
    string out = "{\"type\":\"ROOM_MSG\",\"sender\":";
    appendEscaped(out, m.sender);
    out += ",\"roomId\":";
    out += to_string(m.roomId);
    out += ",\"text\":";
    appendEscaped(out, m.text);
    out.push_back('}');
    return out;
}

string privateMsgToJson(const Message& m)
{
    string out = "{\"type\":\"PRIVATE_MSG\",\"sender\":";
    appendEscaped(out, m.sender);
    out += ",\"targetUser\":";
    appendEscaped(out, m.target);
    out += ",\"text\":";
    appendEscaped(out, m.text);
    out.push_back('}');
    return out;
}

string serverError(const string& text)
{
    string out = "{\"type\":\"ERROR\",\"text\":";
    appendEscaped(out, text);
    out.push_back('}');
    return out;
}

string serverNotice(const string& text)
{
    string out = "{\"type\":\"NOTICE\",\"text\":";
    appendEscaped(out, text);
    out.push_back('}');
    return out;
}
