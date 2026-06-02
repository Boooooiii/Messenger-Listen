#ifndef LOGGER_H
#define LOGGER_H

#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

class Logger
{
public:
    enum LogLevel { Info, Warning, Error };

    Logger(const QString& name);
    ~Logger();

    void log(const QString& message, LogLevel level = Info);
private:

    QString getCurrentTimestamp();
    QString transformLogLevelToQString(LogLevel level);

    QFile file;
    QTextStream fileStream;
    QMutex mutex;
};

#endif // LOGGER_H
