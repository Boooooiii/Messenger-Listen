#include "logger.h"

Logger::Logger(const QString& name) {
    file.setFileName(name.contains(".") ? name : name + ".log");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        fileStream.setDevice(&file);

        fileStream << getCurrentTimestamp() << "\t\t" << "LOGGER STARTED\n";
        fileStream.flush();
    }
}

Logger::~Logger() {
    if (file.isOpen()) {
        fileStream << getCurrentTimestamp() + "\t\t" + "LOGGER FINISHED\n\n";
        fileStream.flush();
        file.close();
    }
}

void Logger::log(const QString& message, LogLevel level) {
    QMutexLocker locker(&mutex);

#ifdef QT_DEBUG
    qDebug() << transformLogLevelToQString(level) << "  " << message;
#endif

    if (file.isOpen()) {
        fileStream << getCurrentTimestamp() << transformLogLevelToQString(level) << "   " << message << "\n";
        fileStream.flush();
    }
}

QString Logger::getCurrentTimestamp() {
    return QString("[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + "]\t");
}

QString Logger::transformLogLevelToQString(LogLevel level) {
    if (level == LogLevel::Info) {
        return "INFO";
    } else if (level == LogLevel::Warning) {
        return "WARNING";
    } else if (level == LogLevel::Error) {
        return "ERROR";
    } else {
        return "UNKNOWN";
    }
}
