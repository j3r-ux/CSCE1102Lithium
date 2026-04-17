#include "form.h"
#include "ui_form.h"
#include "settings.h"
#include "login.h"
Form::Form(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
}

Form::~Form()
{
    delete ui;
}

void Form::on_settingsButton_clicked()
{
    Settings *settings = new Settings();
    settings->show();

    this->hide();
}


void Form::on_disconnectpushButton_clicked()
{
    Login *login = new Login();
    login->show();

    this->close();   // close chat
}

