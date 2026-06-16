#ifndef USERSTABLE_H
#define USERSTABLE_H

#include "table.h"

class UsersTable : public Table
{
public:
    explicit UsersTable(QSqlDatabase db) : Table(db) {}

    bool init() override final {
        return 1;
        // here will be SQL script to create ACTUAL table IF NOT EXISTS
    }

    bool existsE(const QString& email) {
        return executeRequest("SELECT id FROM users WHERE email = ?", {email}, true);
    }

    bool existsT(const QString& user_tag) {
        return executeRequest("SELECT id FROM users WHERE user_tag = ?", {user_tag}, true);
    }

    bool addUser(const QString& email, const QString& user_tag, const QString& password_hash, const QString& public_key) {
        return executeRequest("INSERT INTO users (email, user_tag, password_hash, public_key) VALUES (?, ?, ?, ?)", {email, user_tag, password_hash, public_key});
    }

    bool deleteUser(const QString& user_tag) {
        return executeRequest("DELETE FROM users WHERE user_tag = ?", {user_tag});
    }

    bool updateEmail(const QString& email, const QString& user_tag) {
        return executeRequest("UPDATE users SET email = ? WHERE user_tag = ?", {email, user_tag});
    }

    bool updateUserTag(const QString& email, const QString& user_tag) {
        return executeRequest("UPDATE users SET user_tag = ? WHERE email = ?", {user_tag, email});
    }

    bool updatePassword(const QString& user_tag, const QString& password_hash) {
        return executeRequest("UPDATE users SET password_hash = ? WHERE user_tag = ?", {password_hash, user_tag});
    }

    bool checkPasswordT(const QString& user_tag, const QString& password_hash) {
        return executeRequest("SELECT id FROM users WHERE user_tag = ? AND password_hash = ?", {user_tag, password_hash});
    }

    bool checkPasswordE(const QString& email, const QString& password_hash) {
        return executeRequest("SELECT id FROM users WHERE email = ? AND password_hash = ?", {email, password_hash});
    }
};

#endif // USERSTABLE_H
