#ifndef TABLE_H
#define TABLE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class Table {
protected:
    QSqlDatabase m_db;
    QString m_lastError;

    explicit Table(QSqlDatabase db) : m_db(db) {}

    template<typename... Args>
    bool executeRequest(const QString& request, const Args&... values);

public:
    virtual ~Table() = default;
    virtual bool init() = 0;

    QString lastError() const { return m_lastError; }
};


#endif // TABLE_H
