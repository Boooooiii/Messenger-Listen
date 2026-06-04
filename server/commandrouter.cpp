#include "commandrouter.h"

void CommandRouter::registerCommand(CommandType type, HandlerFunc handler) {
    routes[type] = handler;
}

void CommandRouter::route(QSslSocket* clientSocket, const Packet& packet, DatabaseManager* dbManager, Logger* logger) {
    auto it = routes.find(packet.type);
    if (it != routes.end()) {
        it->second(clientSocket, packet, dbManager, logger);
    } else {
        logger->log("Unknown command type received: " + QString::number(static_cast<int>(packet.type)), Logger::LogLevel::Warning);
    }
}
