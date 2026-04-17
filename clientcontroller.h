#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QString>

class ClientController
{
public:
    ClientController();
    QString validateUsername(const QString &username);
    QString validatePassword(const QString &password);
};

#endif
