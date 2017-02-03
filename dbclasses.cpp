#include "dbclasses.h"

User::User()
{
    this->id = -1;
    this->name = "";
    this->nickname = "";
    this->birthdate = QDate();
}

User::User(int id, QString name, QString nickname, QDate birthdate)
{
    this->id = id;
    this->name = name;
    this->nickname = nickname;
    this->birthdate = birthdate;
}

User::User(QString name, QString nickname, QDate birthdate)
{
    this->id = -1;
    this->name = name;
    this->nickname = nickname;
    this->birthdate = birthdate;
}

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
