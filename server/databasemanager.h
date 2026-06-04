#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

#include "userstable.h"
#include "logger.h"

class DatabaseManager
{
public:
    explicit DatabaseManager(int handlerId);

    ~DatabaseManager();

    bool isConnected() const { return db.isOpen(); }
    QString lastError() const { return db.lastError().text(); }

    UsersTable* users() const { return usersTable; }

private:
    QSqlDatabase db;
    UsersTable* usersTable = nullptr;
};

#endif // DATABASEMANAGER_H
