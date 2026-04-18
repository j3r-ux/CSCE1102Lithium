#include "settings.h"
#include "ui_settings.h"
#include "login.h"
#include "form.h"

Settings::Settings(QWidget *parent, ClientController *controller)
    : QWidget(parent)
    , ui(new Ui::Settings)
    , controller(controller)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_backButton_clicked()
{
    Form *chat = new Form(nullptr, controller);
    chat->show();
    this->hide();
}


void Settings::on_disconnectButton_clicked()
{
    Login *login = new Login(nullptr, controller);
    login->show();

    this->close();   // or hide()
}

