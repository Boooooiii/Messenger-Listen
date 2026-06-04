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
}

void Server::initServerCommands() {
    CommandRouter& cRouter = CommandRouter::getInstance();

    cRouter.registerCommand(CommandType::Register, [](QSslSocket* client, const Packet& packet, DatabaseManager* db, Logger* logger) {
        QString email = packet.payload["email"].toString();
        QString userTag = packet.payload["user_tag"].toString();
        QString passwordHash = packet.payload["password_hash"].toString();
        QString publicKey = packet.payload["public_key"].toString();

        logger->log("Command [Register] triggered for user_tag: " + userTag);

        if (db->users()->existsT(userTag)) {
            // return error
        }

        if (db->users()->existsE(email)) {
            // return error
        }

        // temporary for testing
        // later firstly will be added to temporary_users table
        if (db->users()->addUser(email, userTag, passwordHash, publicKey)) {
            logger->log("User " + userTag + " succesfully added to db");
            // return success
        } else {
            logger->log("DB error during registration of " + userTag + ": " + db->lastError());
            //return error
        }

        client->write("ACK_REGISTER");
    });

    cRouter.registerCommand(CommandType::Login, [](QSslSocket* client, const Packet& packet, DatabaseManager* db, Logger* logger) {
        QString tagOrEmail = packet.payload["user_tag"].toString();
        QString passwordHash = packet.payload["password_hash"].toString();

        if (tagOrEmail.isEmpty()) {
            tagOrEmail = packet.payload["email"].toString();

            // later will be added session code sending and checking ip
            if (db->users()->checkPasswordE(tagOrEmail, passwordHash)) {
                logger->log("User " + tagOrEmail + " succesfully loginned into account");
                // return success
            } else {
                logger->log("Wrong password or DB error " + tagOrEmail + ": " + db->lastError());
                // return error
            }
            return;
        }

        // later will be added session code sending and checking ip
        if (db->users()->checkPasswordT(tagOrEmail, passwordHash)) {
            logger->log("User " + tagOrEmail + " succesfully loginned into account");
            // return success
        } else {
            logger->log("Wrong password or DB error " + tagOrEmail + ": " + db->lastError());
            // return error
        }

        client->write("ACK_LOGIN");
    });

    cRouter.registerCommand(CommandType::ConfirmEmail, [](QSslSocket* client, const Packet& packet, DatabaseManager* db, Logger* log) {
        QString cc = packet.payload["confirmation_code"].toString();

        // compare code, give answer

        client->write("ACK_CONFIRM_EMAIL");
    });
}