#ifndef TCPCHATTRANSPORT_H
#define TCPCHATTRANSPORT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>

#include "ichattransport.h"

class ChatClient;

class TcpChatTransport : public QObject, public IChatTransport
{
    Q_OBJECT
public:
    explicit TcpChatTransport(QObject *parent = nullptr);

    void setClient(ChatClient *c) { client = c; }
    void connectToServer(const QString &host, quint16 port);
    void send(const QByteArray &bytes) override { socket.write(bytes); }

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QTcpSocket socket;
    ChatClient *client = nullptr;
    bool connectedOnce = false;
};

#endif
