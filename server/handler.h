#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QSslServer>
#include <QSslSocket>
#include <QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QJsonDocument>

#include "logger.h"
#include "commandrouter.h"
#include "databasemanager.h"

class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(unsigned int handlerID, const QSslConfiguration& readyConfig, QObject *parent = nullptr)
        : QObject(parent), id(handlerID), config(readyConfig), logger("handler_" + QString::number(handlerID)), router(CommandRouter::getInstance()), dbManager(new DatabaseManager(handlerID)) {
        logger.log("Handler " + QString::number(handlerID) + " started");

        if (!dbManager->isConnected()) {
            logger.log("Database connection failed in handler " + QString::number(id) + ": " + dbManager->lastError(), Logger::LogLevel::Error);
        }
    }

    ~Handler() {
        if (dbManager) {
            delete dbManager;
        }
        logger.log("Handler +" + QString::number(id) + " destroying.");
    }

    int getClientCount() const { return clients.size(); }

public slots:
    // Creates an instance of a socket object and handles the initial SSL handshake verification
    void handleNewConnection(qintptr socketDescriptor) {
        QSslSocket* socket = new QSslSocket(this);

        if (socket->setSocketDescriptor(socketDescriptor)) {
            logger.log("Handler " + QString::number(id) + " accepted socket from: " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()));
            clients.append(socket);

            connect(socket, &QSslSocket::sslErrors, this, [socket, this](const QList<QSslError> &errors) {
                QString errorsList;
                for (QSslError error : errors) {
                    errorsList += error.errorString() + "\n";
                }
                logger.log("SSL errors occured for user from" +  socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()) + ":\n" + errorsList);
            });

            setupSocket(socket);

            logger.log("Total clients on this worker: " + QString::number(clients.size()));
        } else {
            socket->deleteLater();
        }
    }

private slots:
    // Triggered asynchronously when safe encrypted bytes arrive from a client
    void onDataReceived() {
        QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
        if (!socket) return;

        QDataStream in(socket);
        in.setVersion(QDataStream::Qt_6_0);

        while (socket->bytesAvailable() > 0) {
            in.startTransaction();

            uint32_t payloadSize = 0;
            uint16_t commandInt = 0;
            QByteArray payloadData;

            in >> payloadSize;
            in >> commandInt;
            in >> payloadData;

            if (!in.commitTransaction()) {
                return;
            }

            Packet packet;
            packet.type = static_cast<CommandType>(commandInt);

            QJsonDocument jsonDoc = QJsonDocument::fromJson(payloadData);
            if (jsonDoc.isObject()) {
                packet.payload = jsonDoc.object();
            }

            packet.requestId = packet.payload["requestId"].toString();

            logger.log("Successfully parsed LPF packet from " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()) +
                       " | Cmd: " + QString::number(commandInt) +
                       " | Size: " + QString::number(payloadSize) + " bytes");

            CommandRouter::getInstance().route(socket, packet, dbManager);
        }

        socket->write("ACK");
    }

    // Handles connection cut-off, clears lists and safely frees socket memory
    void onClientDisconnected() {
        QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
        if (!socket) return;

        logger.log("Client " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()) + " disconnected from handler " + QString::number(id));

        clients.removeOne(socket);
        socket->deleteLater();
    }

private:
    // Injects SSL keys, subscribes to successful handshake signal and starts encryption
    void setupSocket(QSslSocket* socket) {
        socket->setSslConfiguration(config);

        connect(socket, &QSslSocket::encrypted, this, [this, socket]() {
            logger.log("SSL Established for new client " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()));
        });

        connect(socket, &QSslSocket::readyRead, this, &Handler::onDataReceived);
        connect(socket, &QSslSocket::disconnected, this, &Handler::onClientDisconnected);

        socket->startServerEncryption();
    }

    void parseIncomingDate(const QByteArray& data) {

    }

    unsigned int id;
    Logger logger;
    QList<QSslSocket*> clients;
    QSslConfiguration config;
    CommandRouter& router;
    DatabaseManager* dbManager;
};

#endif // HANDLER_H
