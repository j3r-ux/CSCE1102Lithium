#include "tcpchattransport.h"

#include "chatclient.h"

#include <QTimer>

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
    lastHost = host;
    lastPort = port;
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
    // Pre-connect error (e.g. server not running yet): keep retrying asynchronously so the client can start before the server.
    // Mid-session errors are paired with a `disconnected` signal and handled separately.
    if (!connectedOnce) {
        QTimer::singleShot(retryDelayMs, this, [this]() {
            socket.connectToHost(lastHost, lastPort);
        });
    }
}
