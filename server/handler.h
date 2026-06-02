#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QSslServer>
#include <QSslSocket>
#include <QSslCertificate>
#include <QtNetwork/QSslKey>

#include "logger.h"

class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(unsigned int handlerID, const QSslConfiguration& readyConfig, QObject *parent = nullptr)
        : QObject(parent), id(handlerID), config(readyConfig), logger("handler_" + QString::number(handlerID)) {
        logger.log("Handler " + QString::number(handlerID) + " started");
    }

    int getClientCount() const { return clients.size(); }

public slots:
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
    void onDataReceived() {
        QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
        if (!socket) return;

        QByteArray data = socket->readAll();
        logger.log("Received data from client " +  socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()) + " (" + QString::number(data.size()) + " bytes)");

        // Логіка обробки повідомлення месенджера
        socket->write("ACK");
    }

    void onClientDisconnected() {
        QSslSocket* socket = qobject_cast<QSslSocket*>(sender());
        if (!socket) return;

        logger.log("Client " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()) + " disconnected from handler " + QString::number(id));

        clients.removeOne(socket);
        socket->deleteLater();
    }

private:
    void setupSocket(QSslSocket* socket) {
        socket->setSslConfiguration(config);

        connect(socket, &QSslSocket::encrypted, this, [this, socket]() {
            logger.log("SSL Established for new client " + socket->peerAddress().toString() + ":" + QString::number(socket->peerPort()));
        });

        connect(socket, &QSslSocket::readyRead, this, &Handler::onDataReceived);
        connect(socket, &QSslSocket::disconnected, this, &Handler::onClientDisconnected);

        socket->startServerEncryption();
    }

    unsigned int id;
    Logger logger;
    QList<QSslSocket*> clients;
    QSslConfiguration config;
};

#endif // HANDLER_H
