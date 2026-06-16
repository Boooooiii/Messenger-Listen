#include "server.h"

Server::Server() : logger("server.log") {
    logger.log("Main server starting.");

    connectionPool = &HandlerManager::getInstance();
    logger.log("Server starting Connection pool.");
    connectionPool->startConnectionPool();

    initServerCommands();
}

Server::~Server() {
    logger.log("Main server destroying.");
    logger.log("Main server stoping connection pool.");
    connectionPool->stopConnectionPool();

    logger.log("Main server destroyed successfully.");
}

void Server::initServerCommands() {
    CommandRouter& cRouter = CommandRouter::getInstance();

    cRouter.registerCommand(CommandType::Register, [](QSslSocket* client, const Packet& packet, DatabaseManager* db, Logger* logger, Handler* handler) {
        QString email = packet.payload["email"].toString();
        QString userTag = packet.payload["user_tag"].toString();
        QString passwordHash = packet.payload["password_hash"].toString();
        QString publicKey = packet.payload["public_key"].toString();

        logger->log("Command [Register] triggered for user_tag: " + userTag);

        if (db->users()->existsT(userTag)) {
            logger->log("User with user tag " + userTag + " already exists", Logger::LogLevel::Warning);
            handler->sendToClient(client, packet.type, packet.requestId, 0, "User with this Tag already exists");
            return;
        }

        if (db->users()->existsE(email)) {
            logger->log("User with email " + email + " already exists", Logger::LogLevel::Warning);
            handler->sendToClient(client, packet.type, packet.requestId, 0, "User with this Email already exists");
            return;
        }

        // temporary for testing
        // later firstly will be added to temporary_users table
        if (db->users()->addUser(email, userTag, passwordHash, publicKey)) {
            logger->log("User " + userTag + " succesfully added to db");
            handler->sendToClient(client, packet.type, packet.requestId, 1, "User registered successfully");
        } else {
            logger->log("DB error during registration of " + userTag + ": " + db->lastError(), Logger::LogLevel::Error);
            handler->sendToClient(client, packet.type, packet.requestId, 0, "Database error: " + db->lastError());
        }
    });

    cRouter.registerCommand(CommandType::Login, [](QSslSocket* client, const Packet& packet, DatabaseManager* db, Logger* logger, Handler* handler) {
        QString tagOrEmail = packet.payload["user_tag"].toString();
        QString passwordHash = packet.payload["password_hash"].toString();

        if (tagOrEmail.isEmpty()) {
            tagOrEmail = packet.payload["email"].toString();

            logger->log("Command [Login] triggered for email: " + tagOrEmail);

            // later will be added session code sending and checking ip
            if (db->users()->checkPasswordE(tagOrEmail, passwordHash)) {
                logger->log("User " + tagOrEmail + " succesfully logged in into account");
                handler->sendToClient(client, packet.type, packet.requestId, 1, "User logged in successfully");

            } else {
                logger->log(db->lastError().isEmpty() ? ("Wrong password to log in by " + tagOrEmail) : ("DB error" + db->lastError()),
                            db->lastError().isEmpty() ? Logger::LogLevel::Warning                     : Logger::LogLevel::Error);

                handler->sendToClient(client, packet.type, packet.requestId, 0, db->lastError().isEmpty() ? "Wrong password" : ("Database error" + db->lastError()));
            }
            return;
        }

        logger->log("Command [Login] triggered for user_tag: " + tagOrEmail);

        // later will be added session code sending and checking ip
        if (db->users()->checkPasswordT(tagOrEmail, passwordHash)) {
            logger->log("User " + tagOrEmail + " succesfully logged in into account");
            handler->sendToClient(client, packet.type, packet.requestId, 1, "User logged in successfully");
        } else {
            logger->log(db->lastError().isEmpty() ? ("Wrong password to log in by " + tagOrEmail) : ("DB error" + db->lastError()),
                        db->lastError().isEmpty() ? Logger::LogLevel::Warning                     : Logger::LogLevel::Error);

            handler->sendToClient(client, packet.type, packet.requestId, 0, db->lastError().isEmpty() ? "Wrong password" : ("Database error" + db->lastError()));
        }

    });

    cRouter.registerCommand(CommandType::ConfirmEmail, [](QSslSocket* client, const Packet& packet, DatabaseManager* db, Logger* logger, Handler* handler) {
        QString tagOrEmail = packet.payload["user_tag"].toString();
        QString cc = packet.payload["confirmation_code"].toString();

        if (tagOrEmail.isEmpty()) {
            tagOrEmail = packet.payload["email"].toString();
            logger->log("Command [Login] triggered for email: " + tagOrEmail);
        }

        logger->log("Command [Login] triggered for user_tag: " + tagOrEmail);

        // compare code, give answer

        client->write("ACK_CONFIRM_EMAIL");
    });
}