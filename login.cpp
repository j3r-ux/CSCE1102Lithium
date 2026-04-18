#include "login.h"
#include "ui_login.h"
#include "form.h"
#include <QMessageBox>

Login::Login(QWidget *parent, ClientController *controller, ChatClient *chatClient)
    : QMainWindow(parent)
    , ui(new Ui::Login)
    , controller(controller)
    , chatClient(chatClient)
{
    ui->setupUi(this);
}
Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_clicked()
{
    QString username = ui->user_lineedit->text();
    QString password = ui->password_lineedit->text();

        // I don't think this code actually does anything, signIn validates internally i'm pretty sure
    // QString usernameError = controller->validateUsername(username);
    // QString passwordError = controller->validatePassword(password);
    // if (!usernameError.isEmpty()) {
    //     QMessageBox::warning(this, "Error", usernameError);
    //     return;
    // }

    // if (!passwordError.isEmpty()) {
    //     QMessageBox::warning(this, "Error", passwordError);
    //     return;
    // }

    QString message;
    bool ok = controller->signIn(username, password, message);

    if (ok) {
        chatClient->setUsername(username);

        QMessageBox::information(this, "Success", message);


        QMessageBox::information(this, "Success", "Welcome, " + username + "!");
        Form *chat = new Form(nullptr, controller, chatClient, username);
        chat->show();
        this->hide();
    } else {
        QMessageBox::warning(this, "Error", message);
    }

}

void Login::on_SignUp_button_clicked()
{
    {
        QString username = ui->user_lineedit->text();
        QString password = ui->password_lineedit->text();


        QString message;
        bool ok = controller->signUp(username, password, message);

        if (ok) {
            chatClient->setUsername(username);
            QMessageBox::information(this, "Success", message);

            Form *chat = new Form(nullptr, controller, chatClient, username);
            chat->show();
            this->hide();
        } else {
            QMessageBox::warning(this, "Error", message);
        }
}
}
