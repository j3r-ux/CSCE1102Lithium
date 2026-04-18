#include "login.h"
#include "clientcontroller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientController controller;
    Login w(nullptr, &controller);
    w.show();

    return a.exec();
}
