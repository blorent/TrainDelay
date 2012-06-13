#include "dbaccess.h"
#include <QDebug>
#include <QStringList>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>

dbAccess::dbAccess()
{
}

dbAccess::~dbAccess()
{
    m_db.close();
}


bool dbAccess::init()
{
    // using SQLITE driver
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("delays_records");

    if (m_db.open())
    {
        // Try to locate the contacts database.
        // If it is not available create it.
        if (m_db.tables().indexOf("delays") == -1)
        {
            QSqlQuery query(m_db);
            query.prepare("CREATE TABLE delays (id INT(32), date DATE, startPoint VARCHAR(32), endPoint VARCHAR(32), expectedDepartureTime TIME, expectedArrivalTime TIME, startTime TIME, arrivalTime TIME, delay INT(32), trainNumber INT(32))");
            if (!query.exec())
            {
                qCritical() << query.lastError();
                return false;
            }
        }
    }
    else
    {
        qCritical() << m_db.lastError();
        return false;
    }

    return true;
}

bool dbAccess::addRecord(const DelayedTravel dt)
{
    QSqlQuery query(m_db);
    QString request = QString("INSERT INTO delays (id, date, startPoint, endPoint, expectedDepartureTime, expectedArrivalTime, startTime, arrivalTime, delay, trainNumber) VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9'', '%10')").arg(dt.id, dt.date, dt.startPoint, dt.endPoint, dt.expectedDepartureTime, dt.expectedArrivalTime, dt.startTime, dt.arrivalTime, dt.delay, dt.trainNumber);
    query.prepare(request);
    if( !query.exec() )
        qDebug() << query.lastError();
    else
        qDebug( "Inserted!" );

}
