#include "handlermanager.h"

void HandlerManager::startConnectionPool() {
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

void HandlerManager::stopConnectionPool() {
    if (this->isListening()) {
        this->close();
        logger.log("Shutting down connection pool.");
    }
}

void HandlerManager::incomingConnection(qintptr socketDescriptor) {
    logger.log("New incomming connection");

    unsigned int targetHandlerIndex = getNextHandlerNumber();
    Handler* targetHandler = workers[targetHandlerIndex];

    logger.log("Routing connection to Worker " + QString::number(targetHandlerIndex));
    QMetaObject::invokeMethod(targetHandler, "handleNewConnection",
                              Qt::QueuedConnection, Q_ARG(qintptr, socketDescriptor));
}


HandlerManager::HandlerManager(QObject *parent) :  logger("connection_pool") {
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

HandlerManager::~HandlerManager() {
    logger.log("Connection pool destroying.");

    for (QThread* thread : threads) {
        thread->quit();
        thread->wait();
    }
}


unsigned int HandlerManager::getNextHandlerNumber() {
    unsigned int connectedUsers = workers[0]->getClientCount();
    unsigned int handlerNum = 0;
    for (int i = 1; i < workers.size(); i++) {
        if (connectedUsers > workers[i]->getClientCount()) {
            connectedUsers = workers[i]->getClientCount();
            handlerNum = i;
        }
    }
    return handlerNum;
}


void HandlerManager::initCertificate(const QSslConfiguration& yourConfig) {
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