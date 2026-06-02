#ifndef HANDLERMANAGER_H
#define HANDLERMANAGER_H

#include "handler.h"
#include "logger.h"

#include <QSslServer>
#include <QSslConfiguration>

#include <QThread>

#define MINHANDLERS 16
#define MAXHANDLERS 2024

#define CONNECTIONPOOLPORT 5050

class HandlerManager : public QSslServer
{
    Q_OBJECT
public:
    static HandlerManager& getInstance() {
        static HandlerManager instance;
        return instance;
    }

    void startConnectionPool() {
        if (this->isListening()) {
            logger.log("Connection pool is already running.", Logger::LogLevel::Warning);
            return;
        }

        logger.log("Connection pool starting at port " + QString::number(CONNECTIONPOOLPORT));

        if (this->listen(QHostAddress::Any, CONNECTIONPOOLPORT)) {
            logger.log("Connection pool successfully started.", Logger::LogLevel::Info);
        } else {
            logger.log("Failed to start connection pool: " + this->errorString(), Logger::LogLevel::Error);
        }
    }

    void stopConnectionPool() {
        if (this->isListening()) {
            this->close();
            logger.log("Shutting down connection pool.");
        }
    }

protected:
    void incomingConnection(qintptr socketDescriptor) override {
        logger.log("New incomming connection");

        unsigned int targetWorkerIndex = getNextWorkerNumber();
        Handler* targetWorker = workers[targetWorkerIndex];

        logger.log("Routing connection to Worker " + QString::number(targetWorkerIndex));
        QMetaObject::invokeMethod(targetWorker, "handleNewConnection",
            Qt::QueuedConnection, Q_ARG(qintptr, socketDescriptor));
    }
private:
    explicit HandlerManager(QObject *parent = nullptr) :  logger("connection_pool") {
        logger.log("Connection pool created and initializing...");
        initCertificate(config);

        int cpuCores = QThread::idealThreadCount();
        logger.log("Detected " + QString::number(cpuCores) + " CPU cores. Creating threads...");

        for (int i = 0; i < cpuCores; i++) {
            QThread* thread = new QThread(this);
            Handler* worker = new Handler(i, config);

            worker->moveToThread(thread);

            connect(thread, &QThread::finished, worker, &QObject::deleteLater);

            thread->start();

            threads.append(thread);
            workers.append(worker);
        }
        logger.log("Thread pool pre-warmed and locked to CPU architecture.");
    }

    ~HandlerManager() {
        logger.log("Connection pool destroying.");

        for (QThread* thread : threads) {
            thread->quit();
            thread->wait();
        }
    }

    unsigned int getNextWorkerNumber() {
        unsigned int connectedUsers = workers[0]->getClientCount();
        unsigned int workerNum = 0;
        for (int i = 1; i < workers.size(); i++) {
            if (connectedUsers > workers[i]->getClientCount()) {
                connectedUsers = workers[i]->getClientCount();
                workerNum = i;
            }
        }
        return workerNum;
    }

    void initCertificate(const QSslConfiguration& yourConfig) {
        config = QSslConfiguration::defaultConfiguration();

        QFile certFile("../../server.crt");
        if (certFile.open(QIODevice::ReadOnly))
            config.setLocalCertificate(QSslCertificate(certFile.readAll()));
        else {
            logger.log("Certificate file 'server.crt' not found!", Logger::LogLevel::Error);
        }

        QFile keyFile("../../server.key");
        if (keyFile.open(QIODevice::ReadOnly))
            config.setPrivateKey(QSslKey(keyFile.readAll(), QSsl::Rsa));
        else {
            logger.log("Private key file 'server.key' not found!", Logger::LogLevel::Error);
        }
    }

    Logger logger;

    QSslConfiguration config;
    QVector<QThread*> threads;
    QVector<Handler*> workers;
};

#endif // HANDLERMANAGER_H
