#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>

#include "dbclasses.h"

class DataBase
{
public:
    DataBase();
    int getKittyId();
    QSqlError getLastError();
    QVariant addTransaction(QSqlQuery &q, Transaction newTransaction);
    QVariant addEvent(QSqlQuery &q, Event newEvent);
    QVariant addUser(QSqlQuery &q, User newUser);
    QSqlError deleteTransaction(int transactionId);
    QSqlError deleteEvent(int eventId);
    QSqlError deleteUser(int userId);
    Transaction getTransaction(int id);
    Event getEvent(int id);
    User getUser(int id);

    int getNumEventsOfUser(int userId);
    int getNumTransactions(int userId = -1, int eventId = -1);
    double calcAmountKitty(int eventId);
    int calcNumUsers(int eventId);

    //todo these should be private
    QLatin1String getInsertUserQuery();
    QLatin1String getInsertEventQuery();
    QLatin1String getInsertTransactionQuery();

private:
    QSqlError init();
    int kittyId;
    QSqlError lastError;
};

#endif // DATABASE_H
