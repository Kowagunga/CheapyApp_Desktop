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
     * \param email
     * \param password hash
     * \param password salt
     * \param birthdate
     */
    User(int id, QString name = QString(), QString nickname = QString(), QString email = QString(), QString pwdHash = QString(), QString pwdSalt = QString(), QDate birthdate = QDate());
    /*!
     * \brief User constractor without an id (all parameters required)
     * \param name
     * \param nickname
     * \param email
     * \param pwdHash
     * \param pwdSalt
     * \param birthdate
     */
    User(QString name, QString nickname, QString email, QString pwdHash, QString pwdsalt, QDate birthdate);
    /*!
     * \brief User constractor without an id and encrypting own password
     * \param name
     * \param nickname
     * \param password
     * \param birthdate
     */
    User(QString name, QString nickname, QString email, QString password, QDate birthdate);
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
     * \brief Returns email of the User
     * \return email
     */
    QString getEmail() const {return email;}
    /*!
     * \brief Validate email address
     * \return true if valid
     */
    bool validateEmail();
    /*!
     * \brief Get email hash for gravatar
     * \return hash MD5 hash of user email
     */
    QString getEmailHash();
    /*!
     * \brief Returns the hash of the user password
     * \return passwordHash
     */
    QString getPasswordHash() const {return passwordHash;}
    /*!
     * \brief Returns the salt of the user password
     * \return  passwordSalt
     */
    QString getPasswordSalt() const {return passwordSalt;}
    /*!
     * \brief Set password hash and salt for the user
     * \param hash password hash
     * \param salt password salt
     */
    void setPassword(QString hash, QString salt) {this->passwordHash = hash; this->passwordSalt = salt;}
    /*!
     * \brief Generate and store password hash and salt in the object
     * \param password plain text password
     */
    void hashPassword(QString password);
    /*!
     * \brief Checks if the hash of the given password and stored salt matches the stored hash
     * \param password plan text password
     * \return true if password hash matches stored hash
     */
    bool checkPassword(QString password);
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
        result.append(", ").append(email);
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
     * \brief User email
     */
    QString email;
    /*!
     * \brief Hash of user password
     */
    QString passwordHash;
    /*!
     * \brief Salt of user password
     */
    QString passwordSalt;
    /*!
     * \brief User birthdate
     */
    QDate birthdate;
    /*!
     * \brief Generate a password salt (random string)
     * \return password salt
     */
    QString generatePwdSalt();
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
     * \param creation creation date
     * \param admin administrator
     * \param place
     * \param description
     * \param finished
     */
    Event(int id, QString name = QString(), QDate creation = QDate(), User admin = User(), QString place = QString(), QString description = QString(), bool finished = false);
    /*!
     * \brief Event constructor without an id (all parameters required)
     * \param name
     * \param creation creation date
     * \param admin administrator
     * \param place
     * \param description
     * \param finished
     */
    Event(QString name, QDate creation,  User admin, QString place = QString(), QString description = QString(), bool finished = false);
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
     * \brief Returns creation date of the Event
     * \return creationDate
     */
    QDate getCreationDate() const {return creationDate;}
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
        result.append(", from ").append(creationDate.toString("dd.MM.yyyy"));
        result.append(", by ");
        if(admin.getNickname().isEmpty())
            result.append(QString::number(admin.getId()));
        else
            result.append(admin.getNickname());
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
     * \brief Event creation date
     */
    QDate creationDate;
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
                double amount = 0.0, QDate date = QDate(), QString place = QString(), QString description = QString());
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
    Transaction(User userGiving, User userReceiving, Event event, double amount, QDate date, QString place = QString(), QString description = QString());
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
        if(userGiving.getNickname().isEmpty())
            result.append(QString::number(userGiving.getId()));
        else
            result.append(userGiving.getNickname());
        result.append(" payed ").append(QString::number(amount, 'g', 2));
        result.append(" to ");
        if(userReceiving.getNickname().isEmpty())
            result.append(QString::number(userReceiving.getId()));
        else
            result.append(userReceiving.getNickname());
        result.append(" on ").append(date.toString("dd.MM.yyyy"));
        result.append(" for ");
        if(event.getName().isEmpty())
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
