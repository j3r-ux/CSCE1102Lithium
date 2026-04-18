#include "clientcontroller.h"

ClientController::ClientController() {}

QString ClientController::validateUsername(const QString &username)
{
    if (username.isEmpty())
        return "Username cannot be empty.";
    if (username.length() < 3)
        return "Username is too short.";
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
bool ClientController::signUp(const QString &username, const QString &password, QString &message)
{
    QString userError = validateUsername(username);
    if (!userError.isEmpty()) {
        message = userError;
        return false;
    }

    QString passError = validatePassword(password);
    if (!passError.isEmpty()) {
        message = passError;
        return false;
    }

    if (users.find(username) != users.end()) {
        message = "Username already exists.";
        return false;
    }

    users[username] = User(username, password);
    message = "Account created successfully.";
    return true;
}

bool ClientController::signIn(const QString &username, const QString &password, QString &message)
{
    QString userError = validateUsername(username);
    if (!userError.isEmpty()) {
        message = userError;
        return false;
    }

    QString passError = validatePassword(password);
    if (!passError.isEmpty()) {
        message = passError;
        return false;
    }

    auto it = users.find(username);
    if (it == users.end()) {
        message = "User does not exist.";
        return false;
    }

    if (it->second.password() != password) {
        message = "Incorrect password.";
        return false;
    }

    message = "Login successful.";
    return true;
}
