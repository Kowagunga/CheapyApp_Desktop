#ifndef DBCLASSES_H
#define DBCLASSES_H

#include <QtCore>

class User
{
public:
    User();
    User(int id){ this->id = id;}
    User(int id, QString name = "", QString nickname = "", QDate birthdate = QDate());
    User(QString name, QString nickname, QDate birthdate);
    int getId() const {return id;}
    void setId(int id){this->id = id;}
    QString getName() const {return name;}
    QString getNickname() const {return nickname;}
    QDate getBirthdate() const {return birthdate;}
    int getAge();

    operator std::string() const
    {
        QString result;
        result = QString::number(id);
        result.append(", ").append(name);
        result.append(", ").append(nickname);
        result.append(", ").append(birthdate.toString("dd.MM.yyyy"));
        return result.toUtf8().constData();
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
    Event(int id){ this->id = id;}
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

    operator std::string() const
    {
        QString result;
        result = QString::number(id);
        result.append(", ").append(name);
        result.append(", at ").append(place);
        result.append(", from ").append(startDate.toString("dd.MM.yyyy"));
        result.append(", by ").append(admin.getNickname());
        return result.toUtf8().constData();
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

#endif // DBCLASSES_H
