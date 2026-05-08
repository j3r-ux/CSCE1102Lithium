#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "clientcontroller.h"
#include "chatclient.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr, ClientController *controller = nullptr, ChatClient *chatClient = nullptr, const QString &username = QString());
    ~Form();

private slots:
    void on_settingsButton_clicked();
    void on_disconnectpushButton_clicked();
    void on_sendButton_clicked();
    void onChatConnected();
    void onChatDisconnected();
    void onChatError(const QString &err);
    void onRoomMessage(const QString &sender, const QString &text, int roomId);
private:
    Ui::Form *ui;
    ClientController *controller;
    ChatClient *chatClient;
    QString username;
    static int kDefaultRoomId;

};

#endif // FORM_H
