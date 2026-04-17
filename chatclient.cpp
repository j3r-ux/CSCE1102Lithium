#include "chatclient.h"
#include <QCoro/QCoroNetwork>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>

ChatClient::ChatClient(QObject *parent) : QObject(parent)
{
    // Linking reactions to receiving stuff from socket
    connect(&socket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    connect(&socket, &QTcpSocket::disconnected, this, &ChatClient::onDisconnected);
}
QCoro::Task<void> ChatClient::connectToServer(const QString &host, quint16 port)
{
    socket.connectToHost(host, port);
    bool connected = co_await qCoro(socket).waitForConnected();
    if(!connected)
    {
        emit connectionError(socket.errorString());
        co_return;
    }
    else {onConnected();}
}
void ChatClient::joinRoom(const QString &roomId, const QString &username)
{
    sender = username; // Store the user's name for future messages, will be moved to login once done.
    QJsonObject obj;
    obj["type"] = "JOIN"; // JSON join room message.
    obj["roomId"] = roomId;
    sendJson(obj);
}
void ChatClient::sendMessage(int id, const QString &payload)
{
    QJsonObject message;
    message["type"] = "ROOM_MSG";
    message["sender"] = sender;
    message["roomId"] = id;
    message["text"] = payload;
    sendJson(message);
}

void ChatClient::sendMessage(const QString &targetUser, const QString &payload)
{
    QJsonObject message;
    message["type"] = "PRIVATE_MSG";
    message["sender"] = sender;
    message["targetUser"] = targetUser;
    message["text"] = payload;
    sendJson(message);
}

void ChatClient::sendJson(QJsonObject data){
    QJsonDocument messageDoc(data);
    QByteArray bytes = messageDoc.toJson(QJsonDocument::Compact); //converting to QByteArray
    socket.write(bytes + "\n"); // \n is the delimeter, to seperate messages when receiving messages at the server level and conveying them.
}
void ChatClient::onReadyRead()
{
    buffer.append(socket.readAll()); // Read everything that's currently in the buffer
    int index = buffer.indexOf('\n'); // Index of the message's end.
    while (index != -1){
        QByteArray received = buffer.left(index);
        QJsonDocument receivedDoc = QJsonDocument::fromJson(received);

        // Cleaning buffer
        buffer.remove(0, index + 1);

        if (!receivedDoc.isNull() && receivedDoc.isObject()) { //First check to make sure

            QJsonObject receivedObj = receivedDoc.object();

            // Checking message type and emitting correct signal.
            QString type = receivedObj["type"].toString(); // Check and store type of incoming message
            if(type == "ROOM_MSG")
            {
                emit roomMessageReceived(receivedObj["sender"].toString(), receivedObj["text"].toString(), receivedObj["roomId"].toInt());
            }
            else if (type == "PRIVATE_MSG")
            {
                emit privateMessageReceived(receivedObj["sender"].toString(), receivedObj["text"].toString(), receivedObj["targetUser"].toString());
            }
        }
        index = buffer.indexOf('\n'); //Updating signal to make sure no full message is sent, or keep going if there's another message.
    }
}



void ChatClient::onConnected()
{
    emit connected();
}

void ChatClient::onDisconnected()
{
    emit disconnected();
}
