#ifndef HANDLERMANAGER_H
#define HANDLERMANAGER_H

#include "handler.h"
#include "logger.h"

#include <QSslServer>
#include <QSslConfiguration>

#include <QThread>

#define CONNECTIONPOOLPORT 5050

class HandlerManager : public QSslServer
{
    Q_OBJECT
public:
    static HandlerManager& getInstance() {
        static HandlerManager instance;
        return instance;
    }

    // starts listening to new connections and forwarding users between threads
    void startConnectionPool();

    // stops listening for new users
    void stopConnectionPool();

protected:
    // This method overrides QSslServer one
    // Now it works like this:
    // Choosing targer handler where there are fewest connected users
    // Then push to his thread request for calling 'handleNewConnection' function with descriptor in parameters
    void incomingConnection(qintptr socketDescriptor) override;
private:

    // Initializes certificate and key for Handlers
    // decides how many Handlers will be there comparing them with numbeer of logical cores
    // starts threads for handlers
    explicit HandlerManager(QObject *parent = nullptr);

    ~HandlerManager();

    // Simple function to find which handler has less clients
    unsigned int getNextHandlerNumber();
    // Pre-loading certificate and key for handlers
    void initCertificate(const QSslConfiguration& yourConfig);

    Logger logger;

    QSslConfiguration config;
    QVector<QThread*> threads;
    QVector<Handler*> workers;
};

#endif // HANDLERMANAGER_H
