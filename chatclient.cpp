#include "chatclient.h"
#include <QJsonDocument>
#include <QJsonObject>

ChatClient::ChatClient(QObject *parent) : QObject(parent)
{}
void ChatClient::connectToServer(const QString &host, quint16 port)
{}
void ChatClient::joinRoom(const QString &roomId, const QString &username)
{}
void ChatClient::sendMessage(const QString &roomId, const QString &username, const QString &message)
{}


void ChatClient::onConnected()
{
    emit connected();
}

void ChatClient::onDisconnected()
{
    emit disconnected();
}
