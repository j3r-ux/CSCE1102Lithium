#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include "chatclient.h"
#include "clientcontroller.h"

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr, ClientController *controller = nullptr, ChatClient *chatClient = nullptr, const QString &username = QString());
    ~Settings();

private slots:
    void on_backButton_clicked();
    void on_disconnectButton_clicked();

private:
    Ui::Settings *ui;
    ClientController *controller;
    ChatClient *chatClient;
    QString username;
};

#endif // SETTINGS_H
