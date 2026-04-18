#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H
#include "User.h"
#include <QHash>
#include <QString>
#include <unordered_map>

class ClientController
{
public:
    ClientController();

    QString validateUsername(const QString &username);
    QString validatePassword(const QString &password);

    bool signUp(const QString &username, const QString &password, QString &message);
    bool signIn(const QString &username, const QString &password, QString &message);

private:
    std::unordered_map<QString, User> users;
};

#endif // CLIENTCONTROLLER_H
