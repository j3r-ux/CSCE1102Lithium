#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "clientcontroller.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr, ClientController *controller = nullptr);
    ~Form();

private slots:
    void on_settingsButton_clicked();
    void on_disconnectpushButton_clicked();

private:
    Ui::Form *ui;
    ClientController *controller;
};

#endif // FORM_H
