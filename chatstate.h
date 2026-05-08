#ifndef CHATSTATE_H
#define CHATSTATE_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ChatState
{
public:
    typedef int SessionId;

    
    bool attach(SessionId session, const std::string& username);
    void detach(SessionId session);

    
    bool usernameOf(SessionId session, std::string& out) const;
    bool sessionOf(const std::string& username, SessionId& out) const;
    bool isAttached(SessionId session) const;

    void joinRoom(SessionId session, int roomId);
    void leaveRoom(SessionId session, int roomId);

    
    std::vector<SessionId> roomMembers(int roomId,
                                       bool excludeOriginator = false,
                                       SessionId originator = 0) const;

private:
    mutable std::mutex                                          mu_;
    std::unordered_map<SessionId, std::string>                  usernames_;
    std::unordered_map<std::string, SessionId>                  sessions_;
    std::unordered_map<int, std::unordered_set<SessionId>>      members_;
    std::unordered_map<SessionId, std::unordered_set<int>>      joined_;
};

#endif
