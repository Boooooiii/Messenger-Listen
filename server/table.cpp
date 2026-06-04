#include "table.h"

template<typename... Args>
bool Table::executeRequest(const QString& request, const Args&... values) {
    QSqlQuery query(m_db);
    query.prepare(request);

    (query.addBindValue(values), ...);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}
