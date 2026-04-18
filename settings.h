#ifndef SETTINGS_H
#define SETTINGS_H
#include "clientcontroller.h"
#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr, ClientController *controller = nullptr);
    ~Settings();

private slots:
    void on_backButton_clicked();

    void on_disconnectButton_clicked();

private:
    Ui::Settings *ui;
    ClientController *controller;
};

#endif // SETTINGS_H
