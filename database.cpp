#include "database.h"

DataBase::DataBase()
{
    lastError = init();
}

int DataBase::getKittyId()
{
    return kittyId;
}

QSqlError DataBase::getLastError()
{
    return lastError;
}

QSqlError DataBase::init()
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

    if (!q.prepare(getInsertUserQuery()))
        return q.lastError();

    User kitty = User(QLatin1String("Kitty"), QLatin1String("Kitty"), QDate(2000, 1, 1));
    User bruno = User(QLatin1String("Bruno Santamaria"), QLatin1String("Kowagunga"), QDate(1989, 2, 14));
    User xavi = User(QLatin1String("Xavier Parareda"),  QLatin1String("X"), QDate(1989, 7, 30));
    User asustao = User(QLatin1String("Luis Lozano"),  QLatin1String("Asustao"), QDate(1989, 12, 20));

    kitty.setId(addUser(q, kitty).toInt());
    bruno.setId(addUser(q, bruno).toInt());
    xavi.setId(addUser(q, xavi).toInt());
    asustao.setId(addUser(q, asustao).toInt());

    if (!q.prepare(getInsertEventQuery()))
        return q.lastError();

    Event warsaw = Event(QLatin1String("Warsaw Trip"), QDate(2016, 9, 1), QDate(2016, 9, 5), bruno, QLatin1String("Warsaw, Poland"), QLatin1String("Trip to Wasaw to destroy our livers"), 0);
    warsaw.setId(addEvent(q, warsaw).toInt());

    if (!q.prepare(getInsertTransactionQuery()))
        return q.lastError();

    addTransaction(q, Transaction(bruno, asustao, warsaw.getId(), 60.0, QDate(2016,9,2), QLatin1String("Hamburg (Germany)"), QLatin1String("Airbnb 3 nights")));
    addTransaction(q, Transaction(bruno, xavi, warsaw, 85.0, QDate(2016,9,2), QLatin1String("Hamburg (Germany)"), QLatin1String("Airbnb 4 nights")));
    addTransaction(q, Transaction(xavi, kitty, warsaw, 30.0, QDate(2016,9,1), QLatin1String("Warsaw"), QLatin1String("Supermarket Friday")));
    addTransaction(q, Transaction(xavi, kitty, warsaw, 13.0, QDate(2016,9,2), QLatin1String("Warsaw"), QLatin1String("Supermarket Saturday")));
    addTransaction(q, Transaction(asustao, kitty, warsaw, 75.0, QDate(2016,9,2), QLatin1String("Warsaw"), QLatin1String("Cash Friday")));
    addTransaction(q, Transaction(bruno, kitty, warsaw, 75.0, QDate(2016,9,3), QLatin1String("Warsaw"), QLatin1String("Cash Sunday")));
    addTransaction(q, Transaction(bruno, kitty, warsaw, -38.0, QDate(2016,9,5), QLatin1String("Warsaw"), QLatin1String("Cash back Monday")));

    kittyId = kitty.getId();
    return QSqlError();
}

QLatin1String DataBase::getInsertUserQuery()
{
    return QLatin1String("insert into users(name, nickname, birthdate) values(?, ?, ?)");
}

QLatin1String DataBase::getInsertEventQuery()
{
    return QLatin1String("insert into events(name, start, end, place, description, finished, admin) values(?, ?, ?, ?, ?, ?, ?)");
}

QLatin1String DataBase::getInsertTransactionQuery()
{
    return QLatin1String("insert into transactions(usergives, userreceives, event, amount, transactionDate, place, description) values(?, ?, ?, ?, ?, ?, ?)");
}

QVariant DataBase::addTransaction(QSqlQuery &q, Transaction newTransaction)
{
    q.addBindValue(QVariant(newTransaction.getUserGiving().getId()));
    q.addBindValue(QVariant(newTransaction.getUserReceiving().getId()));
    q.addBindValue(QVariant(newTransaction.getEvent().getId()));
    q.addBindValue(newTransaction.getAmount());
    q.addBindValue(newTransaction.getDate());
    q.addBindValue(newTransaction.getPlace());
    q.addBindValue(newTransaction.getDescription());
    q.exec();
    return q.lastInsertId();
}


QVariant DataBase::addEvent(QSqlQuery &q, Event newEvent)
{
    q.addBindValue(newEvent.getName());
    q.addBindValue(newEvent.getStartDate());
    q.addBindValue(newEvent.getEndDate());
    q.addBindValue(newEvent.getPlace());
    q.addBindValue(newEvent.getDescription());
    q.addBindValue(newEvent.isFinished());
    q.addBindValue(QVariant(newEvent.getAdmin().getId()));
    q.exec();
    return q.lastInsertId();
}

QVariant DataBase::addUser(QSqlQuery &q, User newUser)
{
    q.addBindValue(newUser.getName());
    q.addBindValue(newUser.getNickname());
    q.addBindValue(newUser.getBirthdate());
    q.exec();
    return q.lastInsertId();
}


QSqlError DataBase::deleteTransaction(int transactionId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM transactions where id = " + QString::number(transactionId)))
        return q.lastError();
    else
        return QSqlError();
}

QSqlError DataBase::deleteEvent(int eventId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM events where id = " + QString::number(eventId)))
        return q.lastError();
    else
        return QSqlError();
}

QSqlError DataBase::deleteUser(int userId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM users where id = " + QString::number(userId)))
        return q.lastError();
    else
        return QSqlError();
}

void DataBase::getUsersOfTransaction(int idTransaction, int *idUserGives, int *idUserReceives)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;

    transactionModel->setQuery("SELECT usergives, userreceives FROM transactions WHERE id = " + QString::number(idTransaction));


    *idUserGives = transactionModel->data(transactionModel->index(0,0)).toInt();
    *idUserReceives = transactionModel->data(transactionModel->index(0,1)).toInt();

    lastError = transactionModel->lastError();
}

int DataBase::getNumEventsOfUser(int userId)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;
    QString strQuery = "SELECT name FROM events WHERE ";
    strQuery.append("events.admin = " + QString::number(userId));
    transactionModel->setQuery(strQuery);

    lastError = transactionModel->lastError();

    return transactionModel->rowCount();
}

int DataBase::getNumTransactions(int userId, int eventId)
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

    lastError = transactionModel->lastError();

    return transactionModel->rowCount();
}

double DataBase::calcAmountKitty(int eventId)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;

    transactionModel->setQuery("SELECT SUM(amount) FROM transactions WHERE transactions.event = " + QString::number(eventId) +
                               " AND transactions.userreceives = " + QString::number(kittyId));

    lastError = transactionModel->lastError();

    return transactionModel->data(transactionModel->index(0,0)).toDouble();
}

int DataBase::calcNumUsers(int eventId)
{
    QSqlQueryModel *transactionModel;
    transactionModel = new QSqlQueryModel;
    transactionModel->setQuery("SELECT nickname FROM users WHERE users.id IS NOT " + QString::number(kittyId) +
                               " AND (users.id IN "
                                    "(SELECT usergives FROM transactions WHERE transactions.event = "
                                        + QString::number(eventId) + ")" +
                                    " OR users.id IN "
                                    "(SELECT userreceives FROM transactions WHERE transactions.event = "
                                        + QString::number(eventId) + "))");

    lastError = transactionModel->lastError();

    return transactionModel->rowCount();
}
