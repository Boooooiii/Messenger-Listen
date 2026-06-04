#ifndef USERSTABLE_H
#define USERSTABLE_H

#include "table.h"

class UsersTable : public Table
{
public:
    explicit UsersTable(QSqlDatabase db) : Table(db) {}

    bool init() override final {
        1;
    }

    bool existsE(const QString& email) {
        return executeRequest("SELECT id FROM users WHERE email = ?", email);
    }

    bool existsT(const QString& user_tag) {
        return executeRequest("SELECT id FROM users WHERE user_tag = ?", user_tag);
    }

    bool addUser(const QString& email, const QString& user_tag, const QString& password_hash, const QString& public_key) {
        return executeRequest("INSERT INTO users (email, user_tag, password_hash, public_key) VALUES (?, ?, ?, ?)", email, user_tag, password_hash, public_key);
    }

    bool deleteUser(const QString& user_tag) {
        return executeRequest("DELTE FROM users WHERE user_tag = ?", user_tag);
    }

    bool updateEmail(const QString& email) {
        return executeRequest("UPDATE users SET email = ? WHERE email = ?", email, email);
    }

    bool updateUserTag(const QString& user_tag) {
        return executeRequest("UPDATE users SET user_tag = ? WHERE user_tag = ?", user_tag, user_tag);
    }

    bool updatePassword(const QString& user_tag, const QString& password_hash) {
        return executeRequest("UPDATE users SET password_hash = ? WHERE user_tag = ?", password_hash, user_tag);
    }

    bool checkPasswordT(const QString& user_tag, const QString& password_hash) {
        return executeRequest("SELECT id FROM users WHERE user_tag = ? AND password_hash = ?", user_tag, password_hash);
    }

    bool checkPasswordE(const QString& email, const QString& password_hash) {
        return executeRequest("SELECT id FROM users WHERE email = ? AND password_hash = ?", email, password_hash);
    }
};

#endif // USERSTABLE_H
