#ifndef LOGIN_H
#define LOGIN_H
#include "chatclient.h"
#include <QMainWindow>
#include "clientcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr, ClientController *controller = nullptr, ChatClient *chatClient = nullptr);
    ~Login();

private slots:
    void on_pushButton_clicked();

    void on_SignUp_button_clicked();

private:
    Ui::Login *ui;
    ClientController *controller;
    ChatClient *chatClient;
};

#endif // LOGIN_H
