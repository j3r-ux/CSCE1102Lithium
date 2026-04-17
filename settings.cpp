#include "settings.h"
#include "ui_settings.h"
#include "login.h"
#include "form.h"
Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_backButton_clicked()
{
    Form *chat = new Form();
    chat->show();

    this->hide();
}


void Settings::on_disconnectButton_clicked()
{
    Login *login = new Login();
    login->show();

    this->close();   // or hide()
}

