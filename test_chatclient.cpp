#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QSignalSpy>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QString>

#include "chatclient.h"
#include "ichattransport.h"

using ::testing::_;
using ::testing::SaveArg;

namespace {

class MockTransport : public IChatTransport {
public:
    MOCK_METHOD(void, send, (const QByteArray &bytes), (override));
};

QJsonObject parseSentLine(const QByteArray &written)
{
    EXPECT_TRUE(written.endsWith('\n')) << "outbound bytes must end with newline";
    QByteArray trimmed = written;
    if (trimmed.endsWith('\n')) trimmed.chop(1);
    return QJsonDocument::fromJson(trimmed).object();
}

}


TEST(ChatClientTest, ParsesIncomingRoomMessage)
{
    MockTransport transport;
    ChatClient client(&transport);
    QSignalSpy spy(&client, &ChatClient::roomMessageReceived);

    client.feedIncoming(R"({"type":"ROOM_MSG","sender":"alice","roomId":7,"text":"hi room"})" "\n");

    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.first().at(0).toString(), QStringLiteral("alice"));
    EXPECT_EQ(spy.first().at(1).toString(), QStringLiteral("hi room"));
    EXPECT_EQ(spy.first().at(2).toInt(),    7);
}


TEST(ChatClientTest, ParsesIncomingPrivateMessage)
{
    MockTransport transport;
    ChatClient client(&transport);
    QSignalSpy spy(&client, &ChatClient::privateMessageReceived);

    client.feedIncoming(R"({"type":"PRIVATE_MSG","sender":"alice","targetUser":"bob","text":"psst"})" "\n");

    ASSERT_EQ(spy.count(), 1);
    EXPECT_EQ(spy.first().at(0).toString(), QStringLiteral("alice"));
    EXPECT_EQ(spy.first().at(1).toString(), QStringLiteral("psst"));
    EXPECT_EQ(spy.first().at(2).toString(), QStringLiteral("bob"));
}


TEST(ChatClientTest, BuffersPartialAndMultipleMessages)
{
    MockTransport transport;
    ChatClient client(&transport);
    QSignalSpy spy(&client, &ChatClient::roomMessageReceived);

    client.feedIncoming(R"({"type":"ROOM_MSG","sender":"alice")");
    EXPECT_EQ(spy.count(), 0) << "no newline yet, so nothing should be parsed";

    client.feedIncoming(R"(,"roomId":1,"text":"hi"})" "\n"
                        R"({"type":"ROOM_MSG","sender":"bob","roomId":1,"text":"hello"})" "\n");

    ASSERT_EQ(spy.count(), 2);
    EXPECT_EQ(spy.at(0).at(0).toString(), QStringLiteral("alice"));
    EXPECT_EQ(spy.at(1).at(0).toString(), QStringLiteral("bob"));
}

TEST(ChatClientTest, SendsRoomMessageAsJson)
{
    MockTransport transport;
    QByteArray captured;
    EXPECT_CALL(transport, send(_)).WillOnce(SaveArg<0>(&captured));

    ChatClient client(&transport);
    client.setUsername("alice");
    client.sendMessage(7, "hello room");

    const QJsonObject obj = parseSentLine(captured);
    EXPECT_EQ(obj.value("type").toString(),   QStringLiteral("ROOM_MSG"));
    EXPECT_EQ(obj.value("sender").toString(), QStringLiteral("alice"));
    EXPECT_EQ(obj.value("roomId").toInt(),    7);
    EXPECT_EQ(obj.value("text").toString(),   QStringLiteral("hello room"));
}


TEST(ChatClientTest, SendsPrivateMessageAsJson)
{
    MockTransport transport;
    QByteArray captured;
    EXPECT_CALL(transport, send(_)).WillOnce(SaveArg<0>(&captured));

    ChatClient client(&transport);
    client.setUsername("alice");
    client.sendMessage(QStringLiteral("bob"), QStringLiteral("psst"));

    const QJsonObject obj = parseSentLine(captured);
    EXPECT_EQ(obj.value("type").toString(),       QStringLiteral("PRIVATE_MSG"));
    EXPECT_EQ(obj.value("sender").toString(),     QStringLiteral("alice"));
    EXPECT_EQ(obj.value("targetUser").toString(), QStringLiteral("bob"));
    EXPECT_EQ(obj.value("text").toString(),       QStringLiteral("psst"));
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
