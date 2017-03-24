#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>

#include "dbclasses.h"

//! @brief Database class
class DataBase
{
public:
    //! @brief Database constructor
    DataBase();
    /*!
     * @brief Returns the id of the kitty
     * @return kitty id
     */
    int getKittyId();
    /*!
     * @brief Returns the last occurred error in the database
     * @return database error of last query
     */
    QSqlError getLastError();
    /*!
     * @brief Adds transaction object to database
     * @param q New transaction query
     * @param newTransaction New transaction object
     * @return Id of added transaction
     * \sa Transaction()
     */
    QVariant addTransaction(QSqlQuery &q, Transaction newTransaction);
    /*!
     * @brief Adds event object to database
     * @param q New event query
     * @param newEvent New event object
     * @return Id of added event
     * \sa Event()
     */
    QVariant addEvent(QSqlQuery &q, Event newEvent);
    /*!
     * @brief Adds user object to database
     * @param q New user query
     * @param newUser New user object
     * @return Id of added user
     * \sa User()
     */
    QVariant addUser(QSqlQuery &q, User newUser);
    /*!
     * @brief Deletes transaction from database
     * @param transactionId Transaction to be deleted
     * @return Sql error
     */
    QSqlError deleteTransaction(int transactionId);
    /*!
     * @brief Deletes event from database
     * @param eventId event to be deleted
     * @return Sql error
     */
    QSqlError deleteEvent(int eventId);
    /*!
     * @brief Deletes user from database
     * @param userId user to be deleted
     * @return Sql error
     */
    QSqlError deleteUser(int userId);
    /*!
     * @brief Returns transaction with that id
     * @param id Transaction id
     * @return Transaction object
     */
    Transaction getTransaction(int id);
    /*!
     * @brief Returns event with that id
     * @param id Event id
     * @return Event object
     */
    Event getEvent(int id);
    /*!
     * @brief Returns user with that id
     * @param id User id
     * @return User object
     */
    User getUser(int id);
    /*!
     * @brief Returns how many events a user is taking part in
     * @param userId User id
     * @return Number of events
     */
    int getNumEventsOfUser(int userId);
    /*!
     * @brief Returns how many transactions does a user and/or an event have
     * @param userId User id (-1 or not given to match any user)
     * @param eventId Event id (-1 or not given to match any event)
     * @return Number of transactions
     */
    int getNumTransactions(int userId = -1, int eventId = -1);
    /*!
     * @brief Calculates the total amount of the Kitty for an event
     * @param eventId Event id
     * @return Amount of money
     */
    double calcAmountKitty(int eventId);
    /*!
     * @brief Returns the number of users with transactions in an event
     * @param eventId Event id
     * @return Number of users
     */
    int calcNumUsers(int eventId);

    /*!
     * @brief Returns sql query to insert a user in the database
     * @return sql query
     * \todo this should be private
     */
    QLatin1String getInsertUserQuery();
    /*!
     * @brief Returns sql query to insert an event in the database
     * @return sql query
     * \todo this should be private
     */
    QLatin1String getInsertEventQuery();
    /*!
     * @brief Returns sql query to insert a transaction in the database
     * @return sql query
     * \todo this should be private
     */
    QLatin1String getInsertTransactionQuery();

private:
    /*!
     * @brief Initializes the database
     * @return Sql error
     */
    QSqlError init();
    /*!
     * \brief Id of the kitty
     */
    int kittyId;
    /*!
     * \brief Last sql error
     */
    QSqlError lastError;
};

#endif // DATABASE_H
