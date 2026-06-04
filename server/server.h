#ifndef SERVER_H
#define SERVER_H

#include "logger.h"
#include "handlermanager.h"
#include "commandrouter.h"

class Server
{
public:
    Server();
    ~Server();
private:
    void initServerCommands();

    Logger logger;
    HandlerManager* connectionPool;
};

#endif // SERVER_H
