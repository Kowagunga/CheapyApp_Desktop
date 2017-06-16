#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtSql>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include "database.h"

namespace Ui {
class MainWindow;
}

//! \brief The MainWindow class
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * \brief MainWindow constructor
     * \param parent parent widget
     */
    explicit MainWindow(QWidget *parent = 0);
    /*!
     * \brief MainWindow destructor
     */
    ~MainWindow();

public slots:
private slots:
    void showTable(); //!\brief Shows database entries on the tableView
    void updateTransactionUserGiving(); //! \brief Load users giving for the selected event
    void updateTransactionUserReceiving(); //! \brief Load users receiving for the selected event and user giving
    void updateTransactionAmount(); //! \brief Calculate the total amount exchanged between selected users giving and receiving
    void newUser(); //! \brief Create a new user with the information introduced in a dialog
    void newEvent(); //! \brief Create a new event with the information introduced in a dialog
    void newTransaction(); //! \brief Create a new transaction with the information introduced in a dialog
    void deleteUser(); //! \brief Delete an existing user chosen in a dialog
    void deleteEvent(); //! \brief Delete an existing event chosen in a dialog
    void deleteTransaction(); //! \brief Delete an existing transaction chosen in a dialog
    void deleteDatabase(); //! \brief Delete database
    void initExampleDatabase(); //! \brief Trigger example data insertion to the database
    void importDatabase(); //! \brief Import database from file
    void exportDatabase(); //! \brief Export database to file
    void showAboutDialog(); //! \brief Show About Dialog with information about this app
    /*!
     * \brief Triggers an action when a tab is selected
     * \param index index of the current tab
     */
    void tabSelected(int index);
    /*!
     * \brief Shows the selected transaction on the table in the combo-boxes
     * \param index selected transaction as index of the table model
     */
    void selectTransaction(QModelIndex index);
    /*!
     * \brief Show User Profile
     * \param index selected row as index of the table model
     */
    void showUser(QModelIndex index);
    /*!
     * \brief Gravatar download finished
     * \param image
     */
    void gravatarDownloaded(QNetworkReply *reply);

private:
    /*!
     * \brief User interface of MainWindow
     */
    Ui::MainWindow *ui;
    /*!
     * \brief SQL Database
     */
    DataBase db;
    /*!
     * \brief Check if database actions from menu are enabled depending on the database status
     */
    void checkDatabaseActions();
    /*!
     * \brief Shows an SQL error to the User
     * \param err SQL Error
     */
    void showError(const QSqlError &err);
    /*!
     * \brief Temporal model to store database queries
     */
    QSqlRelationalTableModel *globalModel;
    /*!
     * \brief Load transactions on the calculations tab
     */
    void loadTransactions();
    /*!
     * \brief Returns the database-id of the selected item of a combo-box
     * \param cmbBox Combobox
     * \return id
     */
    int getIdFromCmb(QComboBox *cmbBox);
    /*!
     * \brief Changes the current index of the comboBox to the one matching an id
     * \param cmbBox Combobox
     * \param id
     */
    void selectIdInCmb(QComboBox *cmbBox, int id);
    /*!
     * \brief Selects row of a tableView matching a given-id
     * \param tableView tableView with associated model
     * \param id matching id
     */
    void selectRowInTable(QTableView *tableView, int id);
    /*!
     * \brief Selects row of a tableView matching a user giving and receiving pair
     * \param tableView tableView with associated model
     * \param userGives user giving
     * \param userReceives user receiving
     */
    void selectRowInTransactionTable(QTableView *tableView, QString userGives, QString userReceives);
    /*!
     * \brief Load users from database as entries of a combo-box
     * \param cmbBox combo-box
     * \param includeKitty true if kitty should be shown as well
     * \param condition condition given to the query as "WHERE" clause
     * \return true if query returns no results
     */
    bool loadUsersToCmb(QComboBox *cmbBox, bool includeKitty, QString condition);
    /*!
     * \brief Load events from database as entries of a combo-box
     * \param cmbBox combo-box
     * \param condition condition given to the query as "WHERE" clause
     * \return true if query returns no results
     */
    bool loadEventsToCmb(QComboBox *cmbBox, QString condition);
    /*!
     * \brief Load users from database to a table-view
     * \param tableView table-view where data is going to be shown
     * \param condition condition given to the query as "WHERE" clause
     * \return true if query returns no results
     */
    bool loadUsersToTable(QTableView *tableView, QString condition = QString());
    /*!
     * \brief Load events from database to a table-view
     * \param tableView table-view where data is going to be shown
     * \param condition condition given to the query as "WHERE" clause
     * \return true if query returns no results
     */
    bool loadEventsToTable(QTableView *tableView, QString condition = QString());
    /*!
     * \brief Load transactions from database to a table-view
     * \param tableView table-view where data is going to be shown
     * \param showKitty true if transactions with kitty shouw be shown
     * \param showPersonal true if transactions between users should be shown
     * \param condition condition given to the query as "WHERE" clause
     * \return true if query returns no results
     */
    bool loadTransactionsToTable(QTableView *tableView, bool showKitty = true, bool showPersonal = true, QString condition = QString());
};

#endif // MAINWINDOW_H
