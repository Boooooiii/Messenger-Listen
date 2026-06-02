#ifndef SERVER_H
#define SERVER_H

#include "logger.h"
#include "handlermanager.h"

class Server
{
public:
    Server() : logger("server.log") {
        logger.log("Main server starting.");

        connectionPool = &HandlerManager::getInstance();
        connectionPool->startConnectionPool();
        logger.log("Server started Connection pool.");
    }

    ~Server() {
        logger.log("Main server destroying.");
    }

private:
    Logger logger;
    HandlerManager* connectionPool;
};

#endif // SERVER_H
