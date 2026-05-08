#include "chatclient.h"

#include <QJsonDocument>

ChatClient::ChatClient(IChatTransport *transport, QObject *parent)
    : QObject(parent)
    , transport(transport)
{
}

void ChatClient::joinRoom(const QString &roomId, const QString &username)
{
    sender = username;
    QJsonObject obj;
    obj["type"]   = "JOIN";
    obj["roomId"] = roomId;
    sendJson(obj);
}

void ChatClient::sendMessage(int id, const QString &payload)
{
    QJsonObject obj;
    obj["type"]   = "ROOM_MSG";
    obj["sender"] = sender;
    obj["roomId"] = id;
    obj["text"]   = payload;
    sendJson(obj);
}

void ChatClient::sendMessage(const QString &targetUser, const QString &payload)
{
    QJsonObject obj;
    obj["type"]       = "PRIVATE_MSG";
    obj["sender"]     = sender;
    obj["targetUser"] = targetUser;
    obj["text"]       = payload;
    sendJson(obj);
}

void ChatClient::sendJson(const QJsonObject &data)
{
    QJsonDocument doc(data);
    transport->send(doc.toJson(QJsonDocument::Compact) + "\n");
}

void ChatClient::feedIncoming(const QByteArray &bytes)
{
    buffer.append(bytes);
    int index = buffer.indexOf('\n');
    while (index != -1) {
        const QByteArray line = buffer.left(index);
        buffer.remove(0, index + 1);

        const QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isNull() && doc.isObject()) {
            const QJsonObject obj = doc.object();
            const QString type = obj["type"].toString();
            if (type == "ROOM_MSG") {
                emit roomMessageReceived(obj["sender"].toString(),
                                         obj["text"].toString(),
                                         obj["roomId"].toInt());
            } else if (type == "PRIVATE_MSG") {
                emit privateMessageReceived(obj["sender"].toString(),
                                            obj["text"].toString(),
                                            obj["targetUser"].toString());
            }
        }
        index = buffer.indexOf('\n');
    }
}

void ChatClient::notifyConnected()
{
    emit connected();
}

void ChatClient::notifyDisconnected()
{
    emit disconnected();
}

void ChatClient::notifyConnectionError(const QString &message)
{
    emit connectionError(message);
}
