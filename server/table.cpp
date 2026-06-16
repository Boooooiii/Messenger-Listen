#include "table.h"

bool Table::executeRequest(const QString& request, const QVariantList& values, bool expectResult) {
    QSqlQuery query(m_db);
    query.prepare(request);

    for (const QVariant& val : values) {
        query.addBindValue(val);
    }

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (expectResult) {
        return query.next();
    }

    return true;
}