#include "form.h"
#include "ui_form.h"
#include "settings.h"
#include "login.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>

Form::Form(QWidget *parent, ClientController *controller, ChatClient *chatClient, const QString& username)
    : QWidget(parent)
    , ui(new Ui::Form)
    , controller(controller)
    , chatClient(chatClient)
    , username(username)
{
    ui->setupUi(this);

    connect(chatClient, &ChatClient::connected,              this, &Form::onChatConnected);
    connect(chatClient, &ChatClient::disconnected,           this, &Form::onChatDisconnected);
    connect(chatClient, &ChatClient::connectionError,        this, &Form::onChatError);
    connect(chatClient, &ChatClient::roomMessageReceived,    this, &Form::onRoomMessage);
    connect(chatClient, &ChatClient::privateMessageReceived, this, &Form::onPrivateMessage);

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
            this, &Form::on_tabWidget_tabCloseRequested);
    connect(ui->roomsListWidget, &QListWidget::itemDoubleClicked,
            this, &Form::on_roomsListWidget_itemDoubleClicked);
    connect(ui->usersListWidget, &QListWidget::itemDoubleClicked,
            this, &Form::on_usersListWidget_itemDoubleClicked);

    connect(ui->roomIdInput,   &QLineEdit::returnPressed,
            this, &Form::on_joinRoomButton_clicked);
    connect(ui->usernameInput, &QLineEdit::returnPressed,
            this, &Form::on_privateChatButton_clicked);
}

Form::~Form()
{
    delete ui;
}

QString Form::roomKey(int roomId)        { return QStringLiteral("room:%1").arg(roomId); }
QString Form::privateKey(const QString &peer) { return QStringLiteral("user:%1").arg(peer); }

QWidget *Form::createChatPage(const ChatTab &tab)
{
    QWidget *page = new QWidget();
    auto *layout = new QVBoxLayout(page);

    auto *display = new QTextEdit(page);
    display->setReadOnly(true);
    layout->addWidget(display);

    auto *bottom = new QHBoxLayout();
    auto *input  = new QLineEdit(page);
    input->setPlaceholderText("Type a message...");
    auto *sendBtn = new QPushButton("Send", page);
    bottom->addWidget(input);
    bottom->addWidget(sendBtn);
    layout->addLayout(bottom);

    ChatTab stored = tab;
    stored.display = display;
    stored.input   = input;
    tabsByPage.insert(page, stored);
    tabPageByKey.insert(stored.key, page);

    auto sendFn = [this, page]() {
        auto it = tabsByPage.find(page);
        if (it == tabsByPage.end()) return;
        ChatTab &t = it.value();
        const QString text = t.input->text();
        if (text.isEmpty()) return;
        if (t.isPrivate) {
            chatClient->sendMessage(t.peer, text);
        } else {
            chatClient->sendMessage(t.roomId, text);
        }
        appendLine(t.display, username, text);
        t.input->clear();
    };
    connect(sendBtn, &QPushButton::clicked,         this, sendFn);
    connect(input,   &QLineEdit::returnPressed,     this, sendFn);

    return page;
}

QWidget *Form::openRoomTab(int roomId)
{
    const QString key = roomKey(roomId);
    if (auto *existing = tabPageByKey.value(key, nullptr)) {
        ui->tabWidget->setCurrentWidget(existing);
        return existing;
    }

    ChatTab tab;
    tab.key       = key;
    tab.isPrivate = false;
    tab.roomId    = roomId;

    QWidget *page = createChatPage(tab);
    int idx = ui->tabWidget->addTab(page, QStringLiteral("Room %1").arg(roomId));
    ui->tabWidget->setCurrentIndex(idx);

    chatClient->joinRoom(QString::number(roomId), username);
    rememberRoom(roomId);
    return page;
}

QWidget *Form::openPrivateTab(const QString &peer)
{
    if (peer.isEmpty() || peer == username) return nullptr;
    const QString key = privateKey(peer);
    if (auto *existing = tabPageByKey.value(key, nullptr)) {
        ui->tabWidget->setCurrentWidget(existing);
        return existing;
    }

    ChatTab tab;
    tab.key       = key;
    tab.isPrivate = true;
    tab.peer      = peer;

    QWidget *page = createChatPage(tab);
    int idx = ui->tabWidget->addTab(page, QStringLiteral("@%1").arg(peer));
    ui->tabWidget->setCurrentIndex(idx);

    rememberUser(peer);
    return page;
}

void Form::appendLine(QTextEdit *display, const QString &sender, const QString &text)
{
    display->append(sender.toHtmlEscaped() + ": " + text.toHtmlEscaped());
}

void Form::rememberRoom(int roomId)
{
    for (int i = 0; i < ui->roomsListWidget->count(); ++i) {
        if (ui->roomsListWidget->item(i)->data(Qt::UserRole).toInt() == roomId) return;
    }
    auto *item = new QListWidgetItem(QStringLiteral("Room %1").arg(roomId));
    item->setData(Qt::UserRole, roomId);
    ui->roomsListWidget->addItem(item);
}

void Form::rememberUser(const QString &user)
{
    for (int i = 0; i < ui->usersListWidget->count(); ++i) {
        if (ui->usersListWidget->item(i)->text() == user) return;
    }
    ui->usersListWidget->addItem(user);
}

void Form::on_settingsButton_clicked()
{
    Settings *settings = new Settings(nullptr, controller, chatClient, username);
    settings->show();
    this->hide();
}

void Form::on_disconnectpushButton_clicked()
{
    Login *login = new Login(nullptr, controller, chatClient);
    login->show();
    this->close();
}

void Form::on_joinRoomButton_clicked()
{
    bool ok = false;
    const int roomId = ui->roomIdInput->text().trimmed().toInt(&ok);
    if (!ok || roomId <= 0) {
        QMessageBox::warning(this, "Invalid Room", "Please enter a valid numeric room ID.");
        return;
    }
    openRoomTab(roomId);
    ui->roomIdInput->clear();
}

void Form::on_privateChatButton_clicked()
{
    QString peer = ui->usernameInput->text().trimmed();
    if (peer.isEmpty()) {
        if (auto *current = ui->usersListWidget->currentItem()) peer = current->text();
    }
    if (peer.isEmpty()) {
        QMessageBox::warning(this, "No User", "Enter a username or pick one from the list.");
        return;
    }
    if (peer == username) {
        QMessageBox::warning(this, "Invalid User", "You can't private-message yourself.");
        return;
    }
    openPrivateTab(peer);
    ui->usernameInput->clear();
}

void Form::on_tabWidget_tabCloseRequested(int index)
{
    QWidget *page = ui->tabWidget->widget(index);
    if (!page) return;
    auto it = tabsByPage.find(page);
    if (it != tabsByPage.end()) {
        tabPageByKey.remove(it.value().key);
        tabsByPage.erase(it);
    }
    ui->tabWidget->removeTab(index);
    page->deleteLater();
}

void Form::on_roomsListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    bool ok = false;
    int roomId = item->data(Qt::UserRole).toInt(&ok);
    if (!ok || roomId <= 0) roomId = item->text().toInt(&ok);
    if (ok && roomId > 0) openRoomTab(roomId);
}

void Form::on_usersListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    openPrivateTab(item->text());
}

void Form::onChatConnected()
{
    ui->statusLabel->setText("Status: Connected as " + username);
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
    if (sender == username) return;
    QWidget *page = openRoomTab(roomId);
    if (!page) return;
    auto it = tabsByPage.find(page);
    if (it == tabsByPage.end()) return;
    appendLine(it.value().display, sender, text);
}

void Form::onPrivateMessage(const QString &sender, const QString &text, const QString &targetUser)
{
    if (sender == username) return;
    const QString peer = (targetUser == username) ? sender : targetUser;
    QWidget *page = openPrivateTab(peer);
    if (!page) return;
    auto it = tabsByPage.find(page);
    if (it == tabsByPage.end()) return;
    appendLine(it.value().display, sender, text);
}
