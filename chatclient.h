#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>

class ChatClient : public QObject
{
    Q_OBJECT

public:
    explicit ChatClient(QObject *parent = nullptr);

    void connectToServer(const QString &host, quint16 port);
    void joinRoom(const QString &roomId, const QString &username);
    void sendMessage(const QString &roomId, const QString &username, const QString &message);

signals:
    void messageReceived(QString username, QString message);
    void connected();
    void disconnected();

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    QTcpSocket socket;
    QByteArray buffer; // for handling partial messages
};

#endif
