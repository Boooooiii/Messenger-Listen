#include "databasemanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QCoreApplication>

DatabaseManager::DatabaseManager(int handlerId) {
    Logger dbLogger("db_init_" + QString::number(handlerId));
    QString connectionName = "pg_connection_handler_" + QString::number(handlerId);

    db = QSqlDatabase::addDatabase("QPSQL", connectionName);

    QString configPath = QCoreApplication::applicationDirPath() + "../../config.json";
    QFile configFile(configPath);

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        dbLogger.log("Could not open config.json at path: " + configPath, Logger::LogLevel::Error);
        return;
    }

    QByteArray configData = configFile.readAll();
    configFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(configData);
    if (!jsonDoc.isObject()) {
        dbLogger.log("config.json format is invalid (not a JSON object)", Logger::LogLevel::Error);
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    QJsonObject pgObj = rootObj["postgres"].toObject();

    if (pgObj.isEmpty()) {
        dbLogger.log("'postgres' block is missing in config.json", Logger::LogLevel::Error);
        return;
    }

    db.setHostName(pgObj["host"].toString("localhost"));
    db.setDatabaseName(pgObj["database"].toString("Listen"));
    db.setUserName(pgObj["user"].toString("postgres"));
    db.setPassword(pgObj["password"].toString());
    db.setPort(pgObj["port"].toInt(8040));

    if (db.open()) {
        dbLogger.log("Handler " + QString::number(handlerId) + " successfully connected to PostgreSQL.");
        usersTable = new UsersTable(db);
        usersTable->init();
    } else {
        dbLogger.log("Handler " + QString::number(handlerId) + " database error: " + db.lastError().text(), Logger::LogLevel::Error);
    }
}

DatabaseManager::~DatabaseManager() {
    delete usersTable;
    if (db.isOpen()) db.close();
}
