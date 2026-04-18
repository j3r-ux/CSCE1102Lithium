#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>

class ChatClient : public QObject
{
    Q_OBJECT
    public:
        explicit ChatClient(QObject *parent = nullptr);
        QCoro::Task<void> connectToServer(const QString &host, quint16 port);
        void joinRoom(const QString &roomId, const QString &username);
        void sendMessage(int id, const QString &payload);
        void sendMessage(const QString &targetUser, const QString &payload);

    signals:
        void connectionError(QString errorMessage);
        void roomMessageReceived(QString sender, QString text, int roomId);
        void privateMessageReceived(QString sender, QString text, QString sendingUser);
        void connected();
        void disconnected();

    private slots:
        void onReadyRead();
        void onConnected();
        void onDisconnected();

    private:
        QString sender;
        QTcpSocket socket;
        QByteArray buffer; // for handling partial messages
        void sendJson(QJsonObject);

};

#endif
