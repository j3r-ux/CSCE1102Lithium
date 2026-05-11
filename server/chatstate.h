#ifndef CHATSTATE_H
#define CHATSTATE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ChatState
{
public:
    typedef int SessionId; // just looks nicer and is easier to understand, means SessionId is another name for int.

    
    bool attach(SessionId session, const std::string& username); // sets up all the hash map pairs when first attaching, also checks username for duplicates
    void detach(SessionId session); // clears up the hash maps to remove the session

    // Lookup helpers - return true on success and fill the out-parameter.
    bool usernameOf(SessionId session, std::string& out) const;
    bool sessionOf(const std::string& username, SessionId& out) const;
    bool isAttached(SessionId session) const;

    void joinRoom(SessionId session, int roomId);
    void leaveRoom(SessionId session, int roomId);

    // Members of `roomId`. If excludeOriginator is true, `originator` is left out.
    std::vector<SessionId> roomMembers(int roomId, bool excludeOriginator = false, SessionId originator = 0) const;

private:
    std::unordered_map<SessionId, std::string> usernames_;
    std::unordered_map<std::string, SessionId> sessions_;
    std::unordered_map<int, std::unordered_set<SessionId>> members_;
    std::unordered_map<SessionId, std::unordered_set<int>> joined_;
};

#endif
