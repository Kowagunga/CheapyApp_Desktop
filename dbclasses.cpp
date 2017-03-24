#include "dbclasses.h"

/*!
 * Empty User constructor
 */
User::User()
{
    this->id = -1;
    this->name = "";
    this->nickname = "";
    this->birthdate = QDate();
}

/*!
 * User constructor that requires id.
 *
 * The other parameters can be given, but are not required.
 */
User::User(int id, QString name, QString nickname, QDate birthdate)
{
    this->id = id;
    this->name = name;
    this->nickname = nickname;
    this->birthdate = birthdate;
}

/*!
 * User constructor without an id.
 *
 * All parameters are required. After inserting in the database, the
 * resulting id can be set with \sa User::setId(int id)
 */
User::User(QString name, QString nickname, QDate birthdate)
{
    this->id = -1;
    this->name = name;
    this->nickname = nickname;
    this->birthdate = birthdate;
}

/*!
 * Returns the current age of the user in complete years
 */
int User::getAge()
{
    QDate currentDate = QDate::currentDate();
    int currentAge = currentDate.year() - birthdate.year();
    if (birthdate.month() > currentDate.month()
         ||
         (birthdate.month()==currentDate.month()
          &&
          birthdate.day()>currentDate.day()))
    {
        currentAge--;
    }

    return currentAge;
}

/*!
 * Empty Event constructor
 */
Event::Event()
{
    this->id = -1;
    this->name = "";
    this->startDate = QDate();
    this->endDate = QDate();
    this->admin = User();
    this->place = "";
    this->description = "";
    this->finished = false;
}

/*!
 * Event constructor that requires id.
 *
 * The other parameters can be given, but are not required.
 */
Event::Event(int id, QString name, QDate start, QDate end, User admin, QString place, QString description, bool finished)
{
    this->id = id;
    this->name = name;
    this->startDate = start;
    this->endDate = end;
    this->admin = admin;
    this->place = place;
    this->description = description;
    this->finished = finished;
}

/*!
 * Event constructor without an id.
 *
 * All parameters are required. After inserting in the database, the
 * resulting id can be set with \sa Event::setId(int id)
 */
Event::Event(QString name, QDate start, QDate end, User admin, QString place, QString description, bool finished)
{
    this->id = -1;
    this->name = name;
    this->startDate = start;
    this->endDate = end;
    this->admin = admin;
    this->place = place;
    this->description = description;
    this->finished = finished;
}

/*!
 * Empty Transaction constructor
 */
Transaction::Transaction()
{
    this->id = -1;
    this->userGiving = User();
    this->userReceiving = User();
    this->event = Event();
    this->amount = 0;
    this->date = QDate();
    this->place = "";
    this->description = "";
}

/*!
 * Transaction constructor that requires id.
 *
 * The other parameters can be given, but are not required.
 */
Transaction::Transaction(int id, User userGiving, User userReceiving, Event event, double amount, QDate date, QString place, QString description)
{
    this->id = id;
    this->userGiving = userGiving;
    this->userReceiving = userReceiving;
    this->event = event;
    this->amount = amount;
    this->date = date;
    this->place = place;
    this->description = description;
}

/*!
 * Transaction constructor without an id.
 *
 * All parameters are required. After inserting in the database, the
 * resulting id can be set with \sa Transaction::setId(int id)
 */
Transaction::Transaction(User userGiving, User userReceiving, Event event, double amount, QDate date, QString place, QString description)
{
    this->id = -1;
    this->userGiving = userGiving;
    this->userReceiving = userReceiving;
    this->event = event;
    this->amount = amount;
    this->date = date;
    this->place = place;
    this->description = description;
}
