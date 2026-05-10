#include "tcpchattransport.h"

#include "chatclient.h"

TcpChatTransport::TcpChatTransport(QObject *parent)
    : QObject(parent)
{
    connect(&socket, &QTcpSocket::connected, this, &TcpChatTransport::onConnected);
    connect(&socket, &QTcpSocket::readyRead, this, &TcpChatTransport::onReadyRead);
    connect(&socket, &QTcpSocket::disconnected, this, &TcpChatTransport::onDisconnected);
    connect(&socket, &QTcpSocket::errorOccurred, this, &TcpChatTransport::onErrorOccurred);
}

void TcpChatTransport::connectToServer(const QString &host, quint16 port)
{
    socket.connectToHost(host, port);
}

void TcpChatTransport::onConnected()
{
    connectedOnce = true;
    if (client) client->notifyConnected();
}

void TcpChatTransport::onReadyRead()
{
    if (client) client->feedIncoming(socket.readAll());
}

void TcpChatTransport::onDisconnected()
{
    if (client) client->notifyDisconnected();
}

void TcpChatTransport::onErrorOccurred(QAbstractSocket::SocketError)
{
    // Only treat this as a connect failure if we never managed to connect. Mid-session errors come paired with a `disconnected` signal, which is already handled separately.
    if (!connectedOnce && client) {
        client->notifyConnectionError(socket.errorString());
    }
}
