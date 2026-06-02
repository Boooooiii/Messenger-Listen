#include <QCoreApplication>
#include <csignal>

#include <QTimer>

#include "server.h"

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        QCoreApplication::quit();
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    Server server_instance;

    QTimer::singleShot(3000, &a, &QCoreApplication::quit);

    return QCoreApplication::exec();
}
