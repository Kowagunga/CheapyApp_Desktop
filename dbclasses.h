#ifndef DBCLASSES_H
#define DBCLASSES_H

#include <QtCore>
#include <iostream>

//! \brief The User class
class User
{
public:
    //! \brief Empty User constructor
    User();
    /*!
     * \brief User constractor for a given id
     * \param id
     * \param name
     * \param nickname
     * \param birthdate
     */
    User(int id, QString name = "", QString nickname = "", QDate birthdate = QDate());
    /*!
     * \brief User constractor without an id (all parameters required)
     * \param name
     * \param nickname
     * \param birthdate
     */
    User(QString name, QString nickname, QDate birthdate);
    /*!
     * \brief Returns id of the User
     * \return id
     */
    int getId() const {return id;}
    /*!
     * \brief Sets id for the User
     * \param id
     */
    void setId(int id){this->id = id;}
    /*!
     * \brief Returns name of the User
     * \return name
     */
    QString getName() const {return name;}
    /*!
     * \brief Returns nickname of the User
     * \return nickname
     */
    QString getNickname() const {return nickname;}
    /*!
     * \brief Returns birthdate of the User
     * \return birthdate
     */
    QDate getBirthdate() const {return birthdate;}
    /*!
     * \brief Returns age of the User
     * \return age
     */
    int getAge();

    /*!
     * \brief Converts User to QString
     */
    operator QString() const {
        QString result;
        result = QString::number(id);
        result.append(", ").append(name);
        result.append(", ").append(nickname);
        result.append(", ").append(birthdate.toString("dd.MM.yyyy"));
        return result;
    }

private:
    /*!
     * \brief User id
     */
    int id;
    /*!
     * \brief User name
     */
    QString name;
    /*!
     * \brief User nickname
     */
    QString nickname;
    /*!
     * \brief User birthdate
     */
    QDate birthdate;
};

//! \brief The Event class
class Event
{
public:
    //! \brief Empty event constructor
    Event();
    /*!
     * \brief Event constructor for a given id
     * \param id
     * \param name
     * \param start start date
     * \param end end date
     * \param admin administrator
     * \param place
     * \param description
     * \param finished
     */
    Event(int id, QString name = "", QDate start = QDate(), QDate end = QDate(), User admin = User(), QString place = "", QString description = "", bool finished = false);
    /*!
     * \brief Event constructor without an id (all parameters required)
     * \param name
     * \param start start date
     * \param end end date
     * \param admin administrator
     * \param place
     * \param description
     * \param finished
     */
    Event(QString name, QDate start, QDate end,  User admin, QString place = "", QString description = "", bool finished = false);
    /*!
     * \brief Returns id of the Event
     * \return  id
     */
    int getId() const {return id;}
    /*!
     * \brief Sets id for the Event
     * \param id
     */
    void setId(int id){this->id = id;}
    /*!
     * \brief Returns the name of the Event
     * \return name
     */
    QString getName() const {return name;}
    /*!
     * \brief Returns start date of the Event
     * \return startDate
     */
    QDate getStartDate() const {return startDate;}
    /*!
     * \brief Returns end date of the Event
     * \return endDate
     */
    QDate getEndDate() const {return endDate;}
    /*!
     * \brief Returns administrator of the Event
     * \return admin
     */
    User getAdmin() const {return admin;}
    /*!
     * \brief Returns place of the Event
     * \return place
     */
    QString getPlace() const {return place;}
    /*!
     * \brief Returns description of the Event
     * \return description
     */
    QString getDescription() const {return description;}
    /*!
     * \brief Returns true if the Event has been marked as finished and no more transactions are accepted
     * \return finished
     */
    bool isFinished() const {return finished;}
    /*!
     * \brief Sets the Event finished state. False if ongoing, true if finished (no more transactions accepted)
     * \param finished
     */
    void setFinished(bool finished = true) {this->finished = finished;}

    /*!
     * \brief Converts Event to QString
     */
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
    /*!
     * \brief Event id
     */
    int id;
    /*!
     * \brief Event name
     */
    QString name;
    /*!
     * \brief Event start date
     */
    QDate startDate;
    /*!
     * \brief Event end date
     */
    QDate endDate;
    /*!
     * \brief Event administrator
     */
    User admin;
    /*!
     * \brief Event place
     */
    QString place;
    /*!
     * \brief Event description
     */
    QString description;
    /*!
     * \brief Event finished state
     */
    bool finished;
};

//! \brief The Transaction class
class Transaction
{
public:
    //! \brief Empty Transaction constructor
    Transaction();
    /*!
     * \brief Transaction constructor for a given id
     * \param id
     * \param userGiving user who gives/pays money
     * \param userReceiving user who receives money
     * \param event related event of this transaction
     * \param amount amount of money payed
     * \param date
     * \param place
     * \param description
     */
    Transaction(int id, User userGiving = User(), User userReceiving = User(), Event event = Event(),
                double amount = 0.0, QDate date = QDate(), QString place = "", QString description = "");
    /*!
     * \brief Transaction constructor without an id (all parameters required)
     * \param userGiving user who gives/pays money
     * \param userReceiving user who receives money
     * \param event related event of this transaction
     * \param amount amount of money payed
     * \param date
     * \param place
     * \param description
     */
    Transaction(User userGiving, User userReceiving, Event event, double amount, QDate date, QString place = "", QString description = "");
    /*!
     * \brief Returns id of the Transaction
     * \return id
     */
    int getId() const {return id;}
    /*!
     * \brief Sets id for the Transaction
     * \param id
     */
    void setId(int id){this->id = id;}
    /*!
     * \brief Returns the User who gives money in this Transaction
     * \return userGiving
     */
    User getUserGiving() const {return userGiving;}
    /*!
     * \brief Returns the User who receives money in this Transaction
     * \return userReceiving
     */
    User getUserReceiving() const {return userReceiving;}
    /*!
     * \brief Returns the event which this Transaction belongs to
     * \return event
     */
    Event getEvent() const {return event;}
    /*!
     * \brief Returns the amount of money for this Transaction
     * \return amount
     */
    double getAmount() const {return amount;}
    /*!
     * \brief Returns the date of the Transaction
     * \return amount
     */
    QDate getDate() const {return date;}
    /*!
     * \brief Returns the place of the Transaction
     * \return place
     */
    QString getPlace() const {return place;}
    /*!
     * \brief Returns the description of the Transaction
     * \return
     */
    QString getDescription() const {return description;}

    /*!
     * \brief Converts Transaction to QString
     */
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
    /*!
     * \brief Transaction id
     */
    int id;
    /*!
     * \brief User giving money in this Transaction
     */
    User userGiving;
    /*!
     * \brief User receiving money in this Transaction
     */
    User userReceiving;
    /*!
     * \brief Event which this Transaction belongs to
     */
    Event event;
    /*!
     * \brief Transaction amount of money
     */
    double amount;
    /*!
     * \brief Transaction date
     */
    QDate date;
    /*!
     * \brief Transaction place
     */
    QString place;
    /*!
     * \brief Transaction description
     */
    QString description;
};

#endif // DBCLASSES_H
