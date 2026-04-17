#include "login.h"
#include "ui_login.h"
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

    QMessageBox::information(this, "Success", "Welcome, " + username + "!");
}
