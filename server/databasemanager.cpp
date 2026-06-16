#include "databasemanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QCoreApplication>

DatabaseManager::DatabaseManager(int connectionId, Logger* logger) : connectionId(connectionId), logger(logger) {
    QString connectionName = "pg_connection_handler_" + QString::number(connectionId);

    db = QSqlDatabase::addDatabase("QPSQL", connectionName);

    QString configPath = "../../config.json";
    QFile configFile(configPath);

    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        logger->log("Could not open config.json at path: " + configPath, Logger::LogLevel::Error);
        return;
    }

    QByteArray configData = configFile.readAll();
    configFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(configData);
    if (!jsonDoc.isObject()) {
        logger->log("config.json format is invalid (not a JSON object)", Logger::LogLevel::Error);
        return;
    }

    QJsonObject rootObj = jsonDoc.object();
    QJsonObject pgObj = rootObj["postgres"].toObject();

    if (pgObj.isEmpty()) {
        logger->log("'postgres' block is missing in config.json", Logger::LogLevel::Error);
        return;
    }

    db.setHostName(pgObj["host"].toString("localhost"));
    db.setDatabaseName(pgObj["database"].toString("Listen"));
    db.setUserName(pgObj["user"].toString("postgres"));
    db.setPassword(pgObj["password"].toString());
    db.setPort(pgObj["port"].toInt(8040));

    if (db.open()) {
        logger->log("Connection " + QString::number(connectionId) + " successfully connected to PostgreSQL.");
        usersTable = new UsersTable(db);
        usersTable->init();
    } else {
        logger->log("Connection " + QString::number(connectionId) + " database connection error: " + db.lastError().text(), Logger::LogLevel::Error);
    }
}

DatabaseManager::~DatabaseManager() {
    logger->log("Connection " + QString::number(connectionId) + " closing database connection.");
    delete usersTable;
    if (db.isOpen()) db.close();
    logger->log("Connection " + QString::number(connectionId) + " successfully closed database connection.");
}
