#include "settings.h"
#include "ui_settings.h"
#include "login.h"
#include "form.h"

Settings::Settings(QWidget *parent, ClientController *controller, ChatClient *chatClient, const QString &username)
    : QWidget(parent)
    , ui(new Ui::Settings)
    , controller(controller)
    , chatClient(chatClient)
    , username(username)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_backButton_clicked()
{
    Form *chat = new Form(nullptr, controller, chatClient, username);
    chat->show();
    this->hide();
}


void Settings::on_disconnectButton_clicked()
{
    Login *login = new Login(nullptr, controller, chatClient);
    login->show();

    this->close();   // or hide()
}

