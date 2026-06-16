#ifndef COMMANDROUTER_H
#define COMMANDROUTER_H

#include <QObject>
#include <QSslSocket>

#include "logger.h"
#include "databasemanager.h"

#include <QJsonObject>

class Handler;

enum class CommandType : uint16_t {
    Register = 1,
    Login,
    ConfirmEmail,
    Error = 999
};

struct Packet {
    CommandType type;
    QString requestId;
    QJsonObject payload;
};

class CommandRouter : public QObject
{
    Q_OBJECT
public:

    static CommandRouter& getInstance() {
        static CommandRouter instance;
        return instance;
    }

    using HandlerFunc = std::function<void(QSslSocket*, const Packet&, DatabaseManager*, Logger*, Handler*)>;

    void registerCommand(CommandType type, HandlerFunc handler);

    void route(QSslSocket* clientSocket, const Packet& packet, DatabaseManager* dbManager, Logger* logger, Handler* handler);
private:
    CommandRouter() {}

    std::unordered_map<CommandType, HandlerFunc> routes;
};

#endif // COMMANDROUTER_H
