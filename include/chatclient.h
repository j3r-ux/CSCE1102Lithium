#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QByteArray>
#include <QJsonObject>
#include <QString>

#include "ichattransport.h"

class ChatClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatClient(IChatTransport *transport, QObject *parent = nullptr);

    void setUsername(const QString &username) { sender = username; }
    QString username() const { return sender; }
    bool isOnline() const { return isConnected; }
    void login(const QString &username);
    void joinRoom(const QString &roomId, const QString &username);
    void sendMessage(int id, const QString &payload);
    void sendMessage(const QString &targetUser, const QString &payload);

    // Hooks called by the transport in production and by tests offline.
    void feedIncoming(const QByteArray &bytes);
    void notifyConnected();
    void notifyDisconnected();
    void notifyConnectionError(const QString &message);

signals:
    void connectionError(QString errorMessage);
    void roomMessageReceived(QString sender, QString text, int roomId);
    void privateMessageReceived(QString sender, QString text, QString sendingUser);
    void connected();
    void disconnected();

private:
    void sendJson(const QJsonObject &data);
    QString sender;
    IChatTransport *transport;
    QByteArray buffer;
    QString pendingLoginUsername; // queued if login() is called before the transport connects
    bool isConnected = false;
};

#endif
