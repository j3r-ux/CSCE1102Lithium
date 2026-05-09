#include <gtest/gtest.h>

#include "chatstate.h"

#include <algorithm>
#include <string>

static bool contains(const std::vector<int>& vec, int value)
{
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}


//Test in case of duplicate usernames
TEST(ChatStateTest, DuplicateUsernameIsRejected)
{
    ChatState state;

    EXPECT_TRUE (state.attach(1, "ali"));
    EXPECT_FALSE(state.attach(2, "ali"));
    EXPECT_TRUE (state.attach(2, "yousef"));
}


TEST(ChatStateTest, ReattachingSameSessionSucceeds)
{
    ChatState state;

    ASSERT_TRUE(state.attach(1, "ali"));
    EXPECT_TRUE(state.attach(1, "ali"));
    EXPECT_TRUE(state.isAttached(1));
}


TEST(ChatStateTest, DetachClearsUsernameAndRoomMembership)
{
    ChatState state;
    state.attach(1, "ali");
    state.joinRoom(1, 7);
    state.joinRoom(1, 8);

    state.detach(1);

    EXPECT_FALSE(state.isAttached(1));
    EXPECT_TRUE (state.attach(2, "ali")); 
    EXPECT_TRUE (state.roomMembers(7).empty());
    EXPECT_TRUE (state.roomMembers(8).empty());
}


TEST(ChatStateTest, RoomMembersCanExcludeSender)
{
    ChatState state;
    state.joinRoom(1, 42);
    state.joinRoom(2, 42);
    state.joinRoom(3, 42);

    const std::vector<int> all = state.roomMembers(42);
    EXPECT_EQ  (all.size(), 3u);
    EXPECT_TRUE(contains(all, 1));
    EXPECT_TRUE(contains(all, 2));
    EXPECT_TRUE(contains(all, 3));

    const std::vector<int> withoutOne = state.roomMembers(42, true, 1);
    EXPECT_EQ   (withoutOne.size(), 2u);
    EXPECT_FALSE(contains(withoutOne, 1));
    EXPECT_TRUE (contains(withoutOne, 2));
    EXPECT_TRUE (contains(withoutOne, 3));
}


TEST(ChatStateTest, CanLookUpSessionAndUsernameByEither)
{
    ChatState state;
    state.attach(5, "ali");

    std::string name;
    EXPECT_TRUE(state.usernameOf(5, name));
    EXPECT_EQ  (name, "ali");

    int id = 0;
    EXPECT_TRUE(state.sessionOf("ali", id));
    EXPECT_EQ  (id, 5);

    EXPECT_FALSE(state.usernameOf(99, name));
    EXPECT_FALSE(state.sessionOf("nobody", id));
}
