#include "clientcontroller.h"

ClientController::ClientController() {}

QString ClientController::validateUsername(const QString &username)
{
    if (username.isEmpty())
        return "Username cannot be empty";
    if (username.length() < 3)
        return "Username is too short";
    return "";
}

QString ClientController::validatePassword(const QString &password)
{
    if (password.isEmpty())
        return "Password cannot be empty.";
    if (password.length() < 6)
        return "Password is too short.";
    return "";
}
