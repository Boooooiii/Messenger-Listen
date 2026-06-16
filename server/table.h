#ifndef TABLE_H
#define TABLE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantList>

class Table {
protected:
    QSqlDatabase m_db;
    QString m_lastError;

    explicit Table(QSqlDatabase db) : m_db(db) {}

    bool executeRequest(const QString& request, const QVariantList& values = {}, bool expectResult = false);

public:
    virtual ~Table() = default;
    virtual bool init() = 0;

    QString lastError() const { return m_lastError; }
};


#endif // TABLE_H
