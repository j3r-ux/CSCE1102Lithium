#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    User() = default;
    User(const QString &username, const QString &password)
        : m_username(username), m_password(password) {}

    QString username() const { return m_username; }
    QString password() const { return m_password; }

private:
    QString m_username;
    QString m_password;
};

#endif // USER_H
