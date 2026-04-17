#ifndef CHATROOM_H
#define CHATROOM_H

#include <QWidget>

namespace Ui {
class chatroom;
}

class chatroom : public QWidget
{
    Q_OBJECT

public:
    explicit chatroom(QWidget *parent = nullptr);
    ~chatroom();

private:
    Ui::chatroom *ui;
};

#endif // CHATROOM_H
