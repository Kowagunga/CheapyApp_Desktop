#ifndef INITDB_H
#define INITDB_H

#include <QtSql>

int kittyId;

QLatin1String dbGetInsertUserQuery()
{
    return QLatin1String("insert into users(name, nickname, birthdate) values(?, ?, ?)");
}

QLatin1String dbGetInsertEventQuery()
{
    return QLatin1String("insert into events(name, start, end, place, description, finished, admin) values(?, ?, ?, ?, ?, ?, ?)");
}

QLatin1String dbGetInsertTransactionQuery()
{
    return QLatin1String("insert into transactions(usergives, userreceives, event, amount, transactionDate, place, description) values(?, ?, ?, ?, ?, ?, ?)");
}

QVariant dbAddTransaction(QSqlQuery &q, const QVariant &usergives, const QVariant &userreceives, const QVariant &event, double amount, const QDate &transactionDate, const QString &place, const QString &description)
{
    q.addBindValue(usergives);
    q.addBindValue(userreceives);
    q.addBindValue(event);
    q.addBindValue(amount);
    q.addBindValue(transactionDate);
    q.addBindValue(place);
    q.addBindValue(description);
    q.exec();
    return q.lastInsertId();
}


QVariant dbAddEvent(QSqlQuery &q, const QString &name, const QDate &start, const QDate &end, const QString &place, const QString &description, int finished, const QVariant &admin)
{
    q.addBindValue(name);
    q.addBindValue(start);
    q.addBindValue(end);
    q.addBindValue(place);
    q.addBindValue(description);
    q.addBindValue(finished);
    q.addBindValue(admin);
    q.exec();
    return q.lastInsertId();
}

QVariant dbAddUser(QSqlQuery &q, const QString &name, const QString &nick, const QDate &birthdate)
{
    q.addBindValue(name);
    q.addBindValue(nick);
    q.addBindValue(birthdate);
    q.exec();
    return q.lastInsertId();
}

QSqlError dbDeleteTransaction(int transactionId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM transactions where id = " + QString::number(transactionId)))
        return q.lastError();
    else
        return QSqlError();
}

QSqlError dbDeleteEvent(int eventId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM events where id = " + QString::number(eventId)))
        return q.lastError();
    else
        return QSqlError();
}

QSqlError dbDeleteUser(int userId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM users where id = " + QString::number(userId)))
        return q.lastError();
    else
        return QSqlError();
}

void dbGetUsersOfTransaction(int idTransaction, int *idUserGives, int *idUserReceives)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;

    transactionModel->setQuery("SELECT usergives, userreceives FROM transactions WHERE id = " + QString::number(idTransaction));


    *idUserGives = transactionModel->data(transactionModel->index(0,0)).toInt();
    *idUserReceives = transactionModel->data(transactionModel->index(0,1)).toInt();
}

QSqlError dbInit(int *kittyIndex)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("users", Qt::CaseInsensitive)
        && tables.contains("events", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(QLatin1String("create table users(id integer primary key, name varchar, nickname integer, birthdate date)")))
        return q.lastError();
    if (!q.exec(QLatin1String("create table events(id integer primary key, name varchar, start date, end date, place varchar, description varchar, finished integer, admin integer)")))
        return q.lastError();
    if (!q.exec(QLatin1String("create table transactions(id integer primary key, usergives integer, userreceives integer, event integer, amount real, transactionDate date, place varchar, description varchar)")))
        return q.lastError();

    if (!q.prepare(dbGetInsertUserQuery()))
        return q.lastError();
    QVariant kittyId = dbAddUser(q, QLatin1String("Kitty"), QLatin1String("Kitty"), QDate(2000, 1, 1));
    QVariant brunoId = dbAddUser(q, QLatin1String("Bruno Santamaria"), QLatin1String("Kowagunga"), QDate(1989, 2, 14));
    QVariant xaviId = dbAddUser(q, QLatin1String("Xavier Parareda"),  QLatin1String("X"), QDate(1989, 7, 30));
    QVariant sustoId = dbAddUser(q, QLatin1String("Luis Lozano"),  QLatin1String("Asustao"), QDate(1989, 12, 20));

    if (!q.prepare(dbGetInsertEventQuery()))
        return q.lastError();
    QVariant sWarsaw = dbAddEvent(q, QLatin1String("Warsaw Trip"), QDate(2016, 9, 1), QDate(2016, 9, 5), QLatin1String("Warsaw, Poland"), QLatin1String("Trip to Wasaw to destroy our livers"), 0, brunoId);

    if (!q.prepare(dbGetInsertTransactionQuery()))
        return q.lastError();
    dbAddTransaction(q, brunoId, sustoId, sWarsaw, 60.0, QDate(2016,9,2), QLatin1String("Hamburg (Germany)"), QLatin1String("Airbnb 3 nights"));
    dbAddTransaction(q, brunoId, xaviId, sWarsaw, 85.0, QDate(2016,9,2), QLatin1String("Hamburg (Germany)"), QLatin1String("Airbnb 4 nights"));
    dbAddTransaction(q, xaviId, kittyId, sWarsaw, 30.0, QDate(2016,9,1), QLatin1String("Warsaw"), QLatin1String("Supermarket Friday"));
    dbAddTransaction(q, xaviId, kittyId, sWarsaw, 13.0, QDate(2016,9,2), QLatin1String("Warsaw"), QLatin1String("Supermarket Saturday"));
    dbAddTransaction(q, sustoId, kittyId, sWarsaw, 75.0, QDate(2016,9,2), QLatin1String("Warsaw"), QLatin1String("Cash Friday"));
    dbAddTransaction(q, brunoId, kittyId, sWarsaw, 75.0, QDate(2016,9,3), QLatin1String("Warsaw"), QLatin1String("Cash Sunday"));
    dbAddTransaction(q, brunoId, kittyId, sWarsaw, -38.0, QDate(2016,9,5), QLatin1String("Warsaw"), QLatin1String("Cash back Monday"));

    *kittyIndex = kittyId.toInt();
    return QSqlError();
}

int getNumEventsOfUser(int userId)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;
    QString strQuery = "SELECT name FROM events WHERE ";
    strQuery.append("events.admin = " + QString::number(userId));
    transactionModel->setQuery(strQuery);

    return transactionModel->rowCount();
}

int getNumTransactions(int userId = -1, int eventId = -1)
{
    QSqlQueryModel *transactionModel;

    if(userId == -1 && eventId == -1)
        return 0;

    transactionModel = new QSqlQueryModel;
    QString strQuery = "SELECT id FROM transactions WHERE ";
    if(eventId == -1)
        strQuery.append("transactions.userreceives = " + QString::number(userId) +
                        " OR transactions.usergives = " + QString::number(userId));
    else if(userId == -1)
        strQuery.append(" transactions.eventId = " + QString::number(eventId));
    else
        strQuery.append(" transactions.eventId = " + QString::number(eventId) +
                        " AND (transactions.userreceives = " + QString::number(userId) +
                        " OR transactions.usergives = " + QString::number(userId) + ")");
    transactionModel->setQuery(strQuery);

    return transactionModel->rowCount();
}

double calcAmountKitty(int eventId)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;

    transactionModel->setQuery("SELECT SUM(amount) FROM transactions WHERE transactions.event = " + QString::number(eventId) +
                               " AND transactions.userreceives = " + QString::number(kittyId));

//    if(transactionModel->rowCount() == 0)
//        showError(transactionModel->lastError());

    return transactionModel->data(transactionModel->index(0,0)).toDouble();
}

int calcNumUsers(int eventId)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;
    transactionModel->setQuery("SELECT nickname FROM users WHERE users.id IS NOT " + QString::number(kittyId) +
                               " AND users.id IN "
                                    "(SELECT usergives FROM transactions WHERE transactions.event = "
                                        + QString::number(eventId) + ")"); //todo: user also in userreceives

//    if(transactionModel->rowCount() == 0)
//        showError(transactionModel->lastError());

    return transactionModel->rowCount();
}

#endif // INITDB_H
