#ifndef DBCLASSES_H
#define DBCLASSES_H

#include <QtCore>
#include <iostream>

class User
{
public:
    User();
    User(int id, QString name = "", QString nickname = "", QDate birthdate = QDate());
    User(QString name, QString nickname, QDate birthdate);
    int getId() const {return id;}
    void setId(int id){this->id = id;}
    QString getName() const {return name;}
    QString getNickname() const {return nickname;}
    QDate getBirthdate() const {return birthdate;}
    int getAge();

    operator QString() const {
        QString result;
        result = QString::number(id);
        result.append(", ").append(name);
        result.append(", ").append(nickname);
        result.append(", ").append(birthdate.toString("dd.MM.yyyy"));
        return result;
    }

private:
    int id;
    QString name;
    QString nickname;
    QDate birthdate;
};

class Event
{
public:
    Event();
    Event(int id, QString name = "", QDate start = QDate(), QDate end = QDate(), User admin = User(), QString place = "", QString description = "", bool finished = false);
    Event(QString name, QDate start, QDate end,  User admin, QString place = "", QString description = "", bool finished = false);
    int getId() const {return id;}
    void setId(int id){this->id = id;}
    QString getName() const {return name;}
    QDate getStartDate() const {return startDate;}
    QDate getEndDate() const {return endDate;}
    User getAdmin() const {return admin;}
    QString getPlace() const {return place;}
    QString getDescription() const {return description;}
    bool isFinished() const {return finished;}
    void setFinished(bool finished = true) {this->finished = finished;}

    operator QString() const {
        QString result;
        result = QString::number(id);
        result.append(", ").append(name);
        result.append(", at ").append(place);
        result.append(", from ").append(startDate.toString("dd.MM.yyyy"));
        result.append(", by ");
        if(admin.getNickname() == "")
            result.append(admin.getNickname());
        else
            result.append(QString::number(admin.getId()));
        return result;
    }

private:
    int id;
    QString name;
    QDate startDate;
    QDate endDate;
    User admin;
    QString place;
    QString description;
    bool finished;
};

class Transaction
{
public:
    Transaction();
    Transaction(int id, User userGiving = User(), User userReceiving = User(), Event event = Event(),
                double amount = 0.0, QDate date = QDate(), QString place = "", QString description = "");
    Transaction(User userGiving, User userReceiving, Event event, double amount, QDate date, QString place = "", QString description = "");
    int getId() const {return id;}
    void setId(int id){this->id = id;}
    User getUserGiving() const {return userGiving;}
    User getUserReceiving() const {return userReceiving;}
    Event getEvent() const {return event;}
    double getAmount() const {return amount;}
    QDate getDate() const {return date;}
    QString getPlace() const {return place;}
    QString getDescription() const {return description;}

    operator QString() const {
        QString result;
        result = QString::number(id);
        result.append(", ");
        if(userGiving.getNickname() == "")
            result.append(QString::number(userGiving.getId()));
        else
            result.append(userGiving.getNickname());
        result.append(" payed ").append(QString::number(amount, 'g', 2));
        result.append(" to ");
        if(userReceiving.getNickname() == "")
            result.append(QString::number(userReceiving.getId()));
        else
            result.append(userReceiving.getNickname());
        result.append(" on ").append(date.toString("dd.MM.yyyy"));
        result.append(" for ");
        if(event.getName() == "")
            result.append(QString::number(event.getId()));
        else
            result.append(event.getName());
        return result;
    }

private:
    int id;
    User userGiving;
    User userReceiving;
    Event event;
    double amount;
    QDate date;
    QString place;
    QString description;
};

#endif // DBCLASSES_H
