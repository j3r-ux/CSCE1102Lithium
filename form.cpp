#include "form.h"
#include "ui_form.h"
#include "settings.h"
#include "login.h"

Form::Form(QWidget *parent, ClientController *controller, ChatClient *chatClient, const QString& username)
    : QWidget(parent)
    , ui(new Ui::Form)
    , controller(controller)
    , chatClient(chatClient)
    , username(username)
{
    ui->setupUi(this);

    // Setting up default connections to be able to reflect changes in UI.
    connect(chatClient, &ChatClient::connected,             this, &Form::onChatConnected);
    connect(chatClient, &ChatClient::disconnected,          this, &Form::onChatDisconnected);
    connect(chatClient, &ChatClient::connectionError,       this, &Form::onChatError);
    connect(chatClient, &ChatClient::roomMessageReceived,   this, &Form::onRoomMessage);
}

Form::~Form()
{
    delete ui;
}

int Form::kDefaultRoomId = 1; //setting default room number to 1, hard coded for now, to be changed.

void Form::on_settingsButton_clicked()
{
    Settings *settings = new Settings(nullptr, controller, chatClient);
    settings->show();

    this->hide();
}


void Form::on_disconnectpushButton_clicked()
{
    Login *login = new Login(nullptr, controller, chatClient);
    login->show();

    this->close();   // close chat
}

void Form::on_sendButton_clicked()
{
    const QString text = ui->messageInput->text();
    if (text.isEmpty()) return;

    chatClient->sendMessage(kDefaultRoomId, text);
    ui->chatDisplay->append(username + ": " + text.toHtmlEscaped());
    ui->messageInput->clear();
}

void Form::onChatConnected()
{
    ui->statusLabel->setText("Status: Connected");
}

void Form::onChatDisconnected()
{
    ui->statusLabel->setText("Status: Disconnected");
}

void Form::onChatError(const QString &err)
{
    ui->statusLabel->setText("Status: Error - " + err);
}

void Form::onRoomMessage(const QString &sender, const QString &text, int roomId)
{
    ui->chatDisplay->append(sender.toHtmlEscaped() + ": " + text.toHtmlEscaped());
}

