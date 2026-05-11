#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QHash>
#include "clientcontroller.h"
#include "chatclient.h"

class QTextEdit;
class QLineEdit;
class QListWidgetItem;

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
    void on_joinRoomButton_clicked();
    void on_privateChatButton_clicked();
    void on_tabWidget_tabCloseRequested(int index);
    void on_roomsListWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_usersListWidget_itemDoubleClicked(QListWidgetItem *item);

    void onChatConnected();
    void onChatDisconnected();
    void onChatError(const QString &err);
    void onRoomMessage(const QString &sender, const QString &text, int roomId);
    void onPrivateMessage(const QString &sender, const QString &text, const QString &targetUser);

private:
    struct ChatTab {
        QString key;
        bool isPrivate;
        int roomId;
        QString peer;
        QTextEdit *display;
        QLineEdit *input;
    };

    QWidget *openRoomTab(int roomId);
    QWidget *openPrivateTab(const QString &peer);
    QWidget *createChatPage(const ChatTab &tab);
    void appendLine(QTextEdit *display, const QString &sender, const QString &text);
    static QString roomKey(int roomId);
    static QString privateKey(const QString &peer);

    void rememberRoom(int roomId);
    void rememberUser(const QString &user);

    Ui::Form *ui;
    ClientController *controller;
    ChatClient *chatClient;
    QString username;

    QHash<QString, QWidget*> tabPageByKey;
    QHash<QWidget*, ChatTab> tabsByPage;
};

#endif // FORM_H
