#include "chatstate.h"

bool ChatState::attach(SessionId session, const std::string& username)
{
    auto it = sessions_.find(username);
    if (it != sessions_.end() && it->second != session) {
        return false; // username already taken by someone else
    }
    usernames_[session] = username; // adding key-value pairs to both hash tables.
    sessions_[username] = session;
    return true;
}

void ChatState::detach(SessionId session)
{
    //cleaning up the room-session pair
    auto rooms = joined_.find(session); // check which rooms the current session is in
    if (rooms != joined_.end()) {       // check if the current session is even in any room. if it is, run this code to clear it out
        for (int roomId : rooms->second) { 
            auto m = members_.find(roomId); // get the member list
            if (m == members_.end()) continue; // if it's empty (room doesn't exist), skip
            m->second.erase(session); // remove the session from the room
            if (m->second.empty()) members_.erase(m); //if they were the last member, delete the room entirely
        }
        joined_.erase(rooms); //delete this session from the joined hashmap
    }

    // cleaning up the identity
    auto u = usernames_.find(session);
    if (u != usernames_.end()) {
        sessions_.erase(u->second);
        usernames_.erase(u);
    }
}

bool ChatState::usernameOf(SessionId session, std::string& out) const
{
    auto it = usernames_.find(session);   // look up this session in usernames_
    if (it == usernames_.end()) return false; // username not attached, bail
    out = it->second; // it->second is the username, hand it back through the out string (passed by reference)
    return true;
}

bool ChatState::sessionOf(const std::string& username, SessionId& out) const
{
    auto it = sessions_.find(username);   // look up the username in sessions_
    if (it == sessions_.end()) return false; // no one logged in with this name
    out = it->second;                     // it->second is the SessionId, hand it back through the out string (passed by reference)
    return true;
}

bool ChatState::isAttached(SessionId session) const
{
    return usernames_.find(session) != usernames_.end(); // true if this session has a username, false otherwise
}

void ChatState::joinRoom(SessionId session, int roomId)
{
    members_[roomId].insert(session); // add the session to the room's member set
    joined_[session].insert(roomId);  // and add the room to the session's room set
}

void ChatState::leaveRoom(SessionId session, int roomId)
{
    auto m = members_.find(roomId); // find the room's member set
    if (m != members_.end()) { // if the room exists in our tracker, remove the session from it 
        m->second.erase(session);
        if (m->second.empty()) members_.erase(m); // if they were the last one, delete the empty room
    }
    auto j = joined_.find(session); // find this session's room list
    if (j != joined_.end()) j->second.erase(roomId); // if it has one, remove this room from it
}

std::vector<ChatState::SessionId> ChatState::roomMembers(int roomId, bool excludeOriginator, SessionId originator) const
{
    std::vector<SessionId> out;          // result vector we'll return
    auto m = members_.find(roomId);      // find the room's member set
    if (m == members_.end()) return out; // room doesn't exist, return empty
    out.reserve(m->second.size());       // pre-size out to avoid reallocations as we push
    for (SessionId s : m->second) {      // walk every member of the room
        if (excludeOriginator && s == originator) continue; // skip the sender if asked
        out.push_back(s);
    }
    return out;
}
