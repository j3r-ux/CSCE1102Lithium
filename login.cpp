#include "login.h"
#include "ui_login.h"
#include "form.h"
#include <QMessageBox>


Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
    , controller(new ClientController())
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
    delete controller;
}

void Login::on_pushButton_clicked()
{
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();

    QString usernameError = controller->validateUsername(username);
    QString passwordError = controller->validatePassword(password);

    if (!usernameError.isEmpty()) {
        QMessageBox::warning(this, "Error", usernameError);
        return;
    }

    if (!passwordError.isEmpty()) {
        QMessageBox::warning(this, "Error", passwordError);
        return;
    }

    if (users.contains(username)) {
        if (users[username] == password) {
            QMessageBox::information(this, "Success", "Welcome back, " + username + "!");
            Form *chat = new Form();
            chat->show();
            this->hide();
        } else {
            QMessageBox::warning(this, "Error", "Wrong password!");
        }
    } else {
        users[username] = password;
        QMessageBox::information(this, "Registered", "User created: " + username);
        Form *chat = new Form();
        chat->show();
        this->hide();
    }
}
