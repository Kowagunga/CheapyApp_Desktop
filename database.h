#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>

class DataBase
{
public:
    DataBase();
    int getKittyId();
    QSqlError getLastError();
    QVariant addTransaction(QSqlQuery &q, const QVariant &usergives, const QVariant &userreceives, const QVariant &event, double amount, const QDate &transactionDate, const QString &place, const QString &description);
    QVariant addEvent(QSqlQuery &q, const QString &name, const QDate &start, const QDate &end, const QString &place, const QString &description, int finished, const QVariant &admin);
    QVariant addUser(QSqlQuery &q, const QString &name, const QString &nick, const QDate &birthdate);
    QSqlError deleteTransaction(int transactionId);
    QSqlError deleteEvent(int eventId);
    QSqlError deleteUser(int userId);
    void getUsersOfTransaction(int idTransaction, int *idUserGives, int *idUserReceives);
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
