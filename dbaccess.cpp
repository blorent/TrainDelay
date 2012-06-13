#include "dbaccess.h"
#include <QDebug>
#include <QStringList>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

dbAccess::dbAccess()
{
}


bool dbAccess::init()
{
    // using SQLITE driver
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("delays_records");

    if (m_db.open()) {
        // Try to locate the contacts database.
        // If it is not available create it.
        if (m_db.tables().indexOf("delays") == -1) {
            QSqlQuery query(m_db);
            query.prepare("CREATE TABLE delays (id INT(32), date DATE, startPoint VARCHAR(32), endPoint VARCHAR(32), expectedDepartureTime TIME, expectedArrivalTime TIME, startTime TIME, arrivalTime TIME, delay INT(32), trainNumber INT(32))");
            if (!query.exec())
            {
                qCritical() << query.lastError();
                return false;
            }
        }
    } else {
        qCritical() << m_db.lastError();
        return false;
    }

    return true;
}
