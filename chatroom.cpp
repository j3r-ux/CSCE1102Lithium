#include "chatroom.h"
#include "ui_chatroom.h"

chatroom::chatroom(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatroom)
{
    ui->setupUi(this);
}

chatroom::~chatroom()
{
    delete ui;
}
