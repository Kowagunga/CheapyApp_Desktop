#include "database.h"

/*!
 * Database constructor
 */
DataBase::DataBase()
{
    lastError = init();
}

/*!
 * Returns id of the kitty
 */
int DataBase::getKittyId(bool loadFromDb)
{
    if(loadFromDb)
    {
        QSqlQueryModel *model = new QSqlQueryModel;

        model->setQuery("SELECT id FROM users WHERE nickname = 'Kitty'");

        kittyId = model->data(model->index(0,0)).toInt();

        lastError = model->lastError();
    }
    return kittyId;
}

/*!
 * Returns last sql error
 */
QSqlError DataBase::getLastError()
{
    return lastError;
}

/*!
 * Initializes the database
 */
QSqlError DataBase::init()
{
    db = QSqlDatabase::addDatabase("QSQLITE");

    QString path = getDbPath();
    db.setDatabaseName(path);

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("users", Qt::CaseInsensitive)
        && tables.contains("events", Qt::CaseInsensitive)
        && tables.contains("transactions", Qt::CaseInsensitive))
    {
        qDebug() << "Database file present. Tables: " << db.tables();

        kittyId = getKittyId(true);
        return QSqlError();
    }
    else
    {
        //! \todo erase database in case it is an old version, or check version/integrity
        QSqlQuery q;

        qDebug() << "No database file present. Creating tables...";

        if (!q.exec(QLatin1String("create table users("
                                      "id integer primary key, "
                                      "name text, "
                                      "nickname text not null unique, "
                                      "birthdate date"
                                  ")")))
            return q.lastError();
        if (!q.exec(QLatin1String("create table events("
                                      "id integer primary key, "
                                      "name text not null, "
                                      "start date, "
                                      "end date, "
                                      "place text, "
                                      "description text, "
                                      "finished integer not null, "
                                      "admin integer references users(id)"
                                  ")")))
            return q.lastError();
        if (!q.exec(QLatin1String("create table transactions("
                                      "id integer primary key, "
                                      "usergives integer references users(id), "
                                      "userreceives integer references users(id), "
                                      "event integer references events(id), "
                                      "amount real not null, "
                                      "transactionDate date, "
                                      "place text, "
                                      "description text"
                                  ")")))
            return q.lastError();

        if (!q.prepare(getInsertUserQuery()))
            return q.lastError();

        User kitty = User(QLatin1String("Kitty"), QLatin1String("Kitty"), QDate(2000, 1, 1));
        kitty.setId(addUser(q, kitty).toInt());

        kittyId = kitty.getId();
        return QSqlError();
    }
}

/*!
 * Deletes the database
 */
bool DataBase::deleteDb()
{
    // Close database and remove connection
    QString connection;
    connection = db.connectionName();
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(connection);

    QString path = getDbPath();
    return QFile::remove(path);
}

/*!
 * Returns the path to the database file
 * \todo best path to store database?
 */
QString DataBase::getDbPath()
{
    QString path;
//    path.append(QDir::homePath()).append(QDir::separator());
    path.append("CheapyApp.db3");
    path = QDir::toNativeSeparators(path);
    return path;
}

/*!
 * Check if the database has any entries (besides the kitty user)
 */
bool DataBase::isDatabaseEmpty()
{
    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT COUNT(id) FROM users");
    if(model->data(model->index(0,0)).toInt() != 1)
        return false;

    model->setQuery("SELECT COUNT(id) FROM events");
    if(model->data(model->index(0,0)).toInt() != 0)
        return false;

    model->setQuery("SELECT COUNT(id) FROM transactions");
    if(model->data(model->index(0,0)).toInt() != 0)
        return false;

    return true;
}

/*!
 * Insert example data to the database
 */
QSqlError DataBase::initExampleDatabase()
{
    QSqlQuery q;
    if (!q.prepare(getInsertUserQuery()))
        return q.lastError();

    User kitty = getUser(kittyId);

    User bruno = User(QLatin1String("Bruno Santamaria"), QLatin1String("Kowagunga"), QDate(1989, 2, 14));
    User xavi = User(QLatin1String("Xavier Parareda"),  QLatin1String("X"), QDate(1989, 7, 30));
    User asustao = User(QLatin1String("Luis Lozano"),  QLatin1String("Asustao"), QDate(1989, 12, 20));

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
    addTransaction(q, Transaction(kitty, bruno, warsaw, 38.0, QDate(2016,9,5), QLatin1String("Warsaw"), QLatin1String("Cash back Monday")));

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

/*!
 * Inserts new transaction into the database
 */
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
    lastError = q.lastError();
    return q.lastInsertId();
}

/*!
 * Inserts new event into the database
 */
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
    lastError = q.lastError();
    return q.lastInsertId();
}

/*!
 * Inserts new user into the database
 */
QVariant DataBase::addUser(QSqlQuery &q, User newUser)
{
    q.addBindValue(newUser.getName());
    q.addBindValue(newUser.getNickname());
    q.addBindValue(newUser.getBirthdate());
    q.exec();
    lastError = q.lastError();
    return q.lastInsertId();
}

/*!
 * Deletes transaction from the database
 */
QSqlError DataBase::deleteTransaction(int transactionId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM transactions where id = " + QString::number(transactionId)))
        return q.lastError();
    else
        return QSqlError();
}

/*!
 * Deletes event from the database
 */
QSqlError DataBase::deleteEvent(int eventId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM events where id = " + QString::number(eventId)))
        return q.lastError();
    else
        return QSqlError();
}

/*!
 * Deletes user from the database
 */
QSqlError DataBase::deleteUser(int userId)
{
    QSqlQuery q;

    if (!q.exec("DELETE FROM users where id = " + QString::number(userId)))
        return q.lastError();
    else
        return QSqlError();
}

/*!
 * Returns transaction for an id from the database
 */
Transaction DataBase::getTransaction(int id)
{
    Transaction transaction;
    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT * FROM transactions WHERE id = " + QString::number(id));

    transaction = Transaction(id, User(model->data(model->index(0,1)).toInt()),
        User(model->data(model->index(0,2)).toInt()),
        Event(model->data(model->index(0,3)).toInt()),
        model->data(model->index(0,4)).toDouble(),
        model->data(model->index(0,5)).toDate(),
        model->data(model->index(0,6)).toString(),
        model->data(model->index(0,7)).toString());

    lastError = model->lastError();

    return transaction;
}

/*!
 * Returns event for an id from the database
 */
Event DataBase::getEvent(int id)
{
    Event event;
    QSqlQueryModel *model;
    model = new QSqlQueryModel;

    model->setQuery("SELECT * FROM events WHERE id = " + QString::number(id));

    event = Event(id, model->data(model->index(0,1)).toString(),
        model->data(model->index(0,2)).toDate(),
        model->data(model->index(0,3)).toDate(),
        User(model->data(model->index(0,4)).toInt()),
        model->data(model->index(0,5)).toString(),
        model->data(model->index(0,6)).toString(),
        model->data(model->index(0,7)).toBool());

    lastError = model->lastError();

    return event;
}

/*!
 * Returns user for an id from the database
 */
User DataBase::getUser(int id)
{
    User user;
    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT * FROM users WHERE id = " + QString::number(id));

    user = User(id, model->data(model->index(0,1)).toString(),
        model->data(model->index(0,2)).toString(),
        model->data(model->index(0,3)).toDate());

    lastError = model->lastError();

    return user;
}

/*!
 * Returns how many events a user is taking part in
 */
int DataBase::getNumEventsOfUser(int userId)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    QString strQuery = "SELECT name FROM events WHERE ";
    strQuery.append("events.admin = " + QString::number(userId));
    model->setQuery(strQuery);

    lastError = model->lastError();

    return model->rowCount();
}

/*!
 * Returns how many transactions does a user and/or an event have
 */
int DataBase::getNumTransactions(int userId, int eventId)
{
    if(userId == -1 && eventId == -1)//! \todo return total number of transactions if no user nor event
        return 0;

    QSqlQueryModel *model = new QSqlQueryModel;
    QString strQuery = "SELECT id FROM transactions WHERE ";
    if(eventId == -1)
        strQuery.append("transactions.userreceives = " + QString::number(userId) +
                        " OR transactions.usergives = " + QString::number(userId));
    else if(userId == -1)
        strQuery.append("transactions.event = " + QString::number(eventId));
    else
        strQuery.append("transactions.event = " + QString::number(eventId) +
                        " AND (transactions.userreceives = " + QString::number(userId) +
                        " OR transactions.usergives = " + QString::number(userId) + ")");
    model->setQuery(strQuery);

    lastError = model->lastError();

    return model->rowCount();
}

/*!
 * Calculates the total amount of the Kitty for an event
 */
double DataBase::calcAmountKitty(int eventId)
{
    QSqlQueryModel *model = new QSqlQueryModel;

    model->setQuery("SELECT SUM(amount) FROM transactions WHERE transactions.event = " + QString::number(eventId) +
                               " AND transactions.userreceives = " + QString::number(kittyId));

    lastError = model->lastError();

    return model->data(model->index(0,0)).toDouble();
}

/*!
 * Returns the number of users with transactions in an event
 */
int DataBase::calcNumUsers(int eventId)
{
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT nickname FROM users WHERE users.id IS NOT " + QString::number(kittyId) +
                               " AND (users.id IN "
                                    "(SELECT usergives FROM transactions WHERE transactions.event = "
                                        + QString::number(eventId) + ")" +
                                    " OR users.id IN "
                                    "(SELECT userreceives FROM transactions WHERE transactions.event = "
                                        + QString::number(eventId) + "))");

    lastError = model->lastError();

    return model->rowCount();
}
