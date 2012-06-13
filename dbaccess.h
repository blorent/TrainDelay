#ifndef DBACCESS_H
#define DBACCESS_H

#include <QtSql/QSqlDatabase>
#include <QDate>

// Structure that represent a delayed travel
struct DelayedTravel {
    int id; // uniquely identifies a travel (date+number)
    QDate date;
    QString startPoint;
    QString endPoint;
    QTime expectedDepartureTime;
    QTime expectedArrivalTime;
    QTime startTime;
    QTime arrivalTime;
    int delay;
    int trainNumber;
};

class dbAccess
{
public:
    dbAccess();

    bool init();


private:
    QSqlDatabase m_db;
};

#endif // DBACCESS_H
