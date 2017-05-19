#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtSql>
#include <QtDebug>

/*!
 * MainWindow constructor
 *
 * Initializes the UI, the database and connects signals with slots
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString windowTitle = QString("CheapyApp");
    this->setWindowTitle(windowTitle);

    if (!QSqlDatabase::drivers().contains("QSQLITE"))
        QMessageBox::critical(this, "Unable to load database", "This demo needs the SQLITE driver");

    // initialize the database
    if(db.getLastError().type() != QSqlError::NoError) {
        showError(db.getLastError());
        return;
    }

    connect(ui->rbEvents,SIGNAL(clicked(bool)),this,SLOT(showTable()));
    connect(ui->rbUsers,SIGNAL(clicked(bool)),this,SLOT(showTable()));
    connect(ui->rbKittyTransactions,SIGNAL(clicked(bool)),this,SLOT(showTable()));
    connect(ui->rbPersonalTransactions,SIGNAL(clicked(bool)),this,SLOT(showTable()));

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this, SLOT(tabSelected(int)));

    connect(ui->cmbEvent,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTransactionUserGiving()));
    connect(ui->cmbUserGives,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTransactionUserReceiving()));
    connect(ui->cmbUserReceives,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTransactionAmount()));

    connect(ui->actionAddEvent, &QAction::triggered, this, &MainWindow::newEvent);
    connect(ui->actionAddUser, &QAction::triggered, this, &MainWindow::newUser);
    connect(ui->actionAddTransaction, &QAction::triggered, this, &MainWindow::newTransaction);
    connect(ui->actionDeleteEvent, &QAction::triggered, this, &MainWindow::deleteEvent);
    connect(ui->actionDeleteUser, &QAction::triggered, this, &MainWindow::deleteUser);
    connect(ui->actionDeleteTransaction, &QAction::triggered, this, &MainWindow::deleteTransaction);

    connect(ui->actionDeleteDatabase, &QAction::triggered, this, &MainWindow::deleteDatabase);
    connect(ui->actionExampleDatabase, &QAction::triggered, this, &MainWindow::initExampleDatabase);
    connect(ui->actionImportDatabase, &QAction::triggered, this, &MainWindow::importDatabase);
    connect(ui->actionExportDatabase, &QAction::triggered, this, &MainWindow::exportDatabase);

    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);

    connect(ui->tvEventTransactions, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectTransaction(QModelIndex)));

    connect(ui->tvTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(showUser(QModelIndex)));

    checkDatabaseActions(); // Depending on the loaded database, some actions are disabled
}

/*!
 * Check if database actions are enabled or disabled
 *
 * Depending on if the database is empty or not, it can be initialized or exported (if empty),
 * or deleted or exported (if not)
 */
void MainWindow::checkDatabaseActions()
{
    if(db.isDatabaseEmpty())
    {
        ui->actionExampleDatabase->setEnabled(true);
        ui->actionDeleteDatabase->setEnabled(false);
        ui->actionImportDatabase->setEnabled(true);
        ui->actionExportDatabase->setEnabled(false);
    }
    else
    {
        ui->actionExampleDatabase->setEnabled(false);
        ui->actionDeleteDatabase->setEnabled(true);
        ui->actionImportDatabase->setEnabled(false);
        ui->actionExportDatabase->setEnabled(true);
    }
}

/*!
 * Shows an SQL error to the user
 */
void MainWindow::showError(const QSqlError &err)
{
    QMessageBox::critical(this, "Unable to initialize Database",
                          "Error initializing database: " + err.text());
}

/*!
 * MainWindow destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

// _____Tab Tables_____
/*!
 * Shows database entries on the tableView
 */
void MainWindow::showTable()
{
    if(sender() == ui->rbEvents)
    {
        loadEventsToTable(ui->tvTable);

        ui->tvTable->setEditTriggers(0);
        ui->tvTable->setColumnHidden(globalModel->fieldIndex("id"), true);
        ui->tvTable->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if(sender() == ui->rbUsers)
    {
        loadUsersToTable(ui->tvTable);

        ui->tvTable->setEditTriggers(0);
        ui->tvTable->setColumnHidden(globalModel->fieldIndex("id"), true);
        ui->tvTable->setSelectionMode(QAbstractItemView::SingleSelection);
    }
    else if(sender() == ui->rbKittyTransactions)
    {
        loadTransactionsToTable(ui->tvTable, true, false);

        ui->tvTable->setEditTriggers(0);
        ui->tvTable->setSelectionMode(QAbstractItemView::SingleSelection);
//        ui->tvTable->setColumnHidden(2, true);
    }
    else if(sender() == ui->rbPersonalTransactions)
    {
        loadTransactionsToTable(ui->tvTable, false, true);

        ui->tvTable->setEditTriggers(0);
        ui->tvTable->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tvTable->setColumnHidden(2, false);
    }
}

//_____Tab Calculations_____
/*!
 * Loads events to the comboBox and calls the next function to load transactions
 */
void MainWindow::loadTransactions()
{
    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT name, id FROM events WHERE events.id IN (SELECT event FROM transactions) GROUP BY name");

    if(model->rowCount() == 0)
        return;

    bool oldState = ui->cmbEvent->blockSignals(true);
    ui->cmbEvent->setModel(model);
    ui->cmbEvent->blockSignals(oldState);

    updateTransactionUserGiving();
}

/*!
 * Loads users giving to the comboBox and calls the next function to load possible users receiving
 */
void MainWindow::updateTransactionUserGiving()
{
    int eventId = getIdFromCmb(ui->cmbEvent);

    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT nickname, id FROM users WHERE users.id IN "
                                "(SELECT usergives FROM transactions WHERE transactions.event = " + QString::number(eventId) +
                                ") GROUP BY nickname");

    if(model->rowCount() == 0)
        return;

    bool oldState = ui->cmbUserGives->blockSignals(true);
    ui->cmbUserGives->setModel(model);
    ui->cmbUserGives->blockSignals(oldState);

    bool noTransactions = loadTransactionsToTable(ui->tvEventTransactions, true, true, QString("event = " + QString::number(eventId)));

    if(noTransactions)
    {
        QMessageBox msgBox;
        msgBox.setText("There are no transactions in the database.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    ui->tvEventTransactions->setColumnHidden(globalModel->fieldIndex("Event Name"), true);
    ui->tvEventTransactions->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tvEventTransactions->setSelectionBehavior(QAbstractItemView::SelectRows);

    for(int c = 0; c < ui->tvEventTransactions->horizontalHeader()->count(); ++c)
    {
        ui->tvEventTransactions->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }

    ui->sbNumUsers->setValue(db.calcNumUsers(eventId));
    ui->dsbAmountKitty->setValue(db.calcAmountKitty(eventId));

    updateTransactionUserReceiving();
}

/*!
 * Loads users receiving to the comboBox and calculates the total amount between both users
 */
void MainWindow::updateTransactionUserReceiving()
{
    int eventId = getIdFromCmb(ui->cmbEvent);
    int userGivingId = getIdFromCmb(ui->cmbUserGives);

    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT nickname, id FROM users WHERE users.id IN "
                                "(SELECT userreceives FROM transactions WHERE transactions.usergives = " + QString::number(userGivingId) +
                                " AND transactions.event = " + QString::number(eventId) + ") GROUP BY nickname");

    if(model->rowCount() == 0)
        return;

    bool oldState = ui->cmbUserReceives->blockSignals(true);
    ui->cmbUserReceives->setModel(model);
    ui->cmbUserReceives->blockSignals(oldState);

    updateTransactionAmount();
}

/*!
 * Shows the amount of money given from user giving to user receiving minus the amount from user receiving
 * to user giving.
 */
void MainWindow::updateTransactionAmount()
{
    int eventId = getIdFromCmb(ui->cmbEvent);
    int userGivingId = getIdFromCmb(ui->cmbUserGives);
    int userReceivingId = getIdFromCmb(ui->cmbUserReceives);
    double value = 0;

    QSqlQuery query(QString("SELECT SUM(amount) FROM transactions WHERE transactions.event = %1"
                            " AND transactions.usergives = %2 AND transactions.userreceives = %3")
                    .arg(eventId).arg(userGivingId).arg(userReceivingId));
    if(query.next())
        value = query.value(0).toDouble();

    QSqlQuery query2(QString("SELECT SUM(amount) FROM transactions WHERE transactions.event = %1"
                                " AND transactions.usergives = %2 AND transactions.userreceives = %3")
                        .arg(eventId).arg(userReceivingId).arg(userGivingId));
    if(query2.next())
        value -= query2.value(0).toDouble();

    ui->dsbAmount->setValue(value);

    selectRowInTransactionTable(ui->tvEventTransactions, ui->cmbUserGives->currentText(), ui->cmbUserReceives->currentText());
}

// _____Menu bar_____
/*!
 * Create a new user with the information introduced in a dialog
 */
void MainWindow::newUser()
{
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("Create new user");

    QLineEdit *leName = new QLineEdit(&dialog);
    leName->setMaxLength(30);
    form.addRow("Name*:", leName);
    QLineEdit *leNickname = new QLineEdit(&dialog);
    leNickname->setMaxLength(30);
    form.addRow("Nickname*:", leNickname);
    QLineEdit *leEmail = new QLineEdit(&dialog);
    leEmail->setMaxLength(40);
    form.addRow("Email*:", leEmail);

    QLineEdit *lePassword = new QLineEdit(&dialog);
    lePassword->setMaxLength(16);
    lePassword->setEchoMode(QLineEdit::Password);
    form.addRow("Password*:", lePassword);
    QLineEdit *leRepeatPassword = new QLineEdit(&dialog);
    leRepeatPassword->setMaxLength(16);
    leRepeatPassword->setEchoMode(QLineEdit::Password);
    form.addRow("Repeat password*:", leRepeatPassword);

    QDateEdit *deBirthday = new QDateEdit(&dialog);
    deBirthday->setDate(QDate(2000,1,1));
    deBirthday->setDisplayFormat("dd.MM.yyyy");
    deBirthday->setMaximumDate(QDateTime::currentDateTime().date());
    form.addRow("Birthday date:", deBirthday);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        //! \todo check dialog before closing it
        // Trim string fields
        leName->setText(leName->text().trimmed());
        leNickname->setText(leNickname->text().trimmed());
        leEmail->setText(leEmail->text().trimmed());
        leEmail->setText(leEmail->text().toLower());

        User userToAdd = User(leName->text(),leNickname->text(),leEmail->text(),lePassword->text(),deBirthday->date());

        QString problem = QString();
        if(leName->text().isEmpty())
            problem = "The field 'Name' cannot be empty";
        else if(leNickname->text().isEmpty())
            problem = "The field 'Nickname' cannot be empty";
        else if(userToAdd.validateEmail() == false)
            problem = "The email address is not valid";
        else if(lePassword->text().size() < 8)
            problem = "The password must consist of at least 8 characters";
        else if(QString::compare(lePassword->text(),leRepeatPassword->text(),Qt::CaseSensitive)!=0)
            problem = "The repeated password is incorrect";

        if(problem.isEmpty())
        {
            // save User
            QSqlQuery query;
            query.prepare(db.getInsertUserQuery());
            db.addUser(query,userToAdd);

            if(db.getLastError().type() != QSqlError::NoError) {
                showError(db.getLastError());
                return;
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText(QString("User %1 (%2) with email %3 created successfully.")
                               .arg(userToAdd.getNickname()).arg(userToAdd.getName()).arg(userToAdd.getEmail()));
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();
                tabSelected(ui->tabWidget->currentIndex()); // Reload information
            }

            checkDatabaseActions();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(problem);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
    }
}

/*!
 * Create a new event with the information introduced in a dialog
 */
void MainWindow::newEvent()
{
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("Create new event");

    QLineEdit *leName = new QLineEdit(&dialog);
    leName->setMaxLength(20);
    form.addRow("Name*:", leName);
    QLineEdit *lePlace = new QLineEdit(&dialog);
    lePlace->setMaxLength(30);
    form.addRow("Place:", lePlace);
    QLineEdit *leDescription = new QLineEdit(&dialog);
    leDescription->setMaxLength(50);
    form.addRow("Description:", leDescription);
    QComboBox *cmbAdmin = new QComboBox(&dialog);
    bool noUsers = loadUsersToCmb(cmbAdmin,false,QString());
    form.addRow("Admin:", cmbAdmin);

    if(noUsers)
    {
        QMessageBox msgBox;
        msgBox.setText("There are no users in the database. An event needs an administrator.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        //! \todo check dialog before closing it
        // Trim string fields
        leName->setText(leName->text().trimmed());
        lePlace->setText(lePlace->text().trimmed());
        leDescription->setText(leDescription->text().trimmed());

        QString problem = QString();
        if(leName->text().isEmpty())
            problem = "The field 'Name' cannot be empty";

        if(problem.isEmpty())
        {
            // save Event
            QSqlQuery query;
            query.prepare(db.getInsertEventQuery());
            db.addEvent(query, Event(leName->text(), QDateTime::currentDateTime().date(), User(getIdFromCmb(cmbAdmin)), lePlace->text(), leDescription->text(), 0));
            if(db.getLastError().type() != QSqlError::NoError) {
                showError(db.getLastError());
                return;
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText(QString("Event %1 created successfully.")
                               .arg(leName->text()));
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();
                tabSelected(ui->tabWidget->currentIndex()); // Reload information
            }

            checkDatabaseActions();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(problem);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
    }
}

/*!
 * Create a new transaction with the information introduced in a dialog
 */
void MainWindow::newTransaction()
{
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("Create new transaction");

    QComboBox *cmbEvents = new QComboBox(&dialog);
    bool noEvents = loadEventsToCmb(cmbEvents,QString());
    form.addRow("Event:", cmbEvents);
    QComboBox *cmbUserGives = new QComboBox(&dialog);
    bool noUsers = loadUsersToCmb(cmbUserGives,true,QString());
    form.addRow("User giving:", cmbUserGives);
    QComboBox *cmbUserReceives = new QComboBox(&dialog);
    loadUsersToCmb(cmbUserReceives,true,QString());
    form.addRow("User receiving:", cmbUserReceives);
    QDoubleSpinBox *dsbAmount = new QDoubleSpinBox(&dialog);
    form.addRow("Amount:", dsbAmount);
    dsbAmount->setDecimals(2);
    dsbAmount->setMinimum(0.00);
    dsbAmount->setMaximum(100000.00);
    dsbAmount->setSuffix(" " + QString(QChar(8364)));
    QDateEdit *deTransactionDate = new QDateEdit(&dialog);
    deTransactionDate->setDate(QDateTime::currentDateTime().date());
    deTransactionDate->setDisplayFormat("dd.MM.yyyy");
    deTransactionDate->setMaximumDate(QDateTime::currentDateTime().date());
    form.addRow("Transaction date:", deTransactionDate);
    QLineEdit *lePlace = new QLineEdit(&dialog);
    lePlace->setMaxLength(30);
    form.addRow("Place:", lePlace);
    QLineEdit *leDescription = new QLineEdit(&dialog);
    leDescription->setMaxLength(50);
    form.addRow("Description:", leDescription);

    if(noEvents)
    {
        QMessageBox msgBox;
        msgBox.setText("There are no events in the database. A transaction needs a related event.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    if(noUsers)
    {
        QMessageBox msgBox;
        msgBox.setText("There are no users in the database. A transaction needs a user.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }


    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        //! \todo check dialog before closing it
        lePlace->setText(lePlace->text().trimmed());
        leDescription->setText(leDescription->text().trimmed());

        QString problem = QString();
        if(getIdFromCmb(cmbUserGives)==getIdFromCmb(cmbUserReceives))
            problem = "User giving must be different from user receiving";

        if(problem.isEmpty())
        {
            // save Transaction
            QSqlQuery query;
            query.prepare(db.getInsertTransactionQuery());
            db.addTransaction(query, Transaction(User(getIdFromCmb(cmbUserGives)), User(getIdFromCmb(cmbUserReceives)), Event(getIdFromCmb(cmbEvents)),
                           dsbAmount->value(), deTransactionDate->date(), lePlace->text(), leDescription->text()));
            if(db.getLastError().type() != QSqlError::NoError) {
                showError(db.getLastError());
                return;
            }
            else
            {
                QMessageBox msgBox;
                msgBox.setText(QString("Transaction created successfully."));
                msgBox.setIcon(QMessageBox::Information);
                msgBox.exec();
                tabSelected(ui->tabWidget->currentIndex()); // Reload information
            }

            if(ui->tabWidget->currentIndex()==1)
                loadTransactions();

            checkDatabaseActions();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText(problem);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
    }
}

/*!
 * Delete an existing user chosen in a dialog
 */
void MainWindow::deleteUser()
{
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("Delete existing user");

    QComboBox *cmbUser = new QComboBox(&dialog);
    bool noUsers = loadUsersToCmb(cmbUser,false,QString());

    if(noUsers)
    {
        QMessageBox msgBox;
        msgBox.setText("There are no users in the database.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    form.addRow("User:", cmbUser);

    QLineEdit *lePassword = new QLineEdit(&dialog);
    lePassword->setMaxLength(16);
    lePassword->setEchoMode(QLineEdit::Password);
    form.addRow("Password*:", lePassword);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        int transactions, events;
        User userToDelete = db.getUser(getIdFromCmb(cmbUser));
        if((transactions = db.getNumTransactions(getIdFromCmb(cmbUser))))
        {
            QMessageBox msgBox;
            msgBox.setText("The user has " + QString::number(transactions) + " transactions. Delete them first");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        else if((events = db.getNumEventsOfUser(getIdFromCmb(cmbUser))))
        {
            QMessageBox msgBox;
            msgBox.setText("The user is admin of " + QString::number(events) + " events. Delete them first");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        else if(userToDelete.checkPassword(lePassword->text()) == false)
        {
            QMessageBox msgBox;
            msgBox.setText("The password is incorrect.");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        else
        {
            db.deleteUser(getIdFromCmb(cmbUser));
            tabSelected(ui->tabWidget->currentIndex()); // Reload information
            checkDatabaseActions();
        }
    }
}

/*!
 * Delete an existing event chosen in a dialog
 */
void MainWindow::deleteEvent()
{
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("Delete existing event");

    QComboBox *cmbEvent = new QComboBox(&dialog);
    bool noEvents = loadEventsToCmb(cmbEvent,QString());

    if(noEvents)
    {
        QMessageBox msgBox;
        msgBox.setText("There are no events in the database.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    form.addRow("Event:", cmbEvent);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        int transactions;
        if((transactions = db.getNumTransactions(-1,getIdFromCmb(cmbEvent))))
        {
            QMessageBox msgBox;
            msgBox.setText("The event has " + QString::number(transactions) + " transactions. Delete them first");
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        else
        {
            db.deleteEvent(getIdFromCmb(cmbEvent));
            tabSelected(ui->tabWidget->currentIndex()); // Reload information
            checkDatabaseActions();
        }
    }
}

/*!
 * Delete an existing transaction chosen in a dialog
 */
void MainWindow::deleteTransaction()
{
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("Delete existing transaction");

    QTableView *tvTransactions = new QTableView(&dialog);
    bool noTransactions = loadTransactionsToTable(tvTransactions, true, true);

    if(noTransactions)
    {
        QMessageBox msgBox;
        msgBox.setText("There are no transactions in the database.");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
        return;
    }

    tvTransactions->setSelectionMode(QAbstractItemView::SingleSelection);
    tvTransactions->setSelectionBehavior(QAbstractItemView::SelectRows);
    tvTransactions->setEditTriggers(0);
    form.addRow(tvTransactions);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    QObject::connect(tvTransactions, SIGNAL(doubleClicked(QModelIndex)), &dialog, SLOT(accept()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        int idTransaction = globalModel->data(globalModel->index(tvTransactions->currentIndex().row(),0)).toInt();
        db.deleteTransaction(idTransaction);
        tabSelected(ui->tabWidget->currentIndex()); // Reload information
        checkDatabaseActions();
    }
}

/*!
 * Show About Dialog with information about this application (e.g. Version, Date, Author, License)
 */
void MainWindow::showAboutDialog()
{
    QDialog dialog(this);
    QVBoxLayout layout(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("About CheapyApp");

    QString title = QString("CheapyApp Version ") + QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD);
    QLabel *lblTitleAndVersion = new QLabel(title);
    lblTitleAndVersion->setStyleSheet("QLabel { font-weight: bold }");
    layout.addWidget(lblTitleAndVersion);

    QString buildDate = QString::fromLocal8Bit(BUILDDATE);
    QLabel *lblBuildDate = new QLabel("Built on: " + buildDate);
    layout.addWidget(lblBuildDate);

    QLabel *lblAuthor = new QLabel("Created by Kowagunga");
    layout.addWidget(lblAuthor);

    QLabel *lblLicense = new QLabel("Licensed under GNU General Public License v3.0");
    layout.addWidget(lblLicense);

    QLabel *lblGithub = new QLabel("Available at <a href=\"https://github.com/Kowagunga/CheapyApp_Desktop/\">Github</a>");
    lblGithub->setTextFormat(Qt::RichText);
    lblGithub->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lblGithub->setOpenExternalLinks(true);
    layout.addWidget(lblGithub);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok,
                               Qt::Horizontal, &dialog);
    layout.addWidget(&buttonBox);

    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {

    }
}

/*!
 * Delete database with all queries and initialize tables
 */
void MainWindow::deleteDatabase()
{
    QMessageBox msgBox;
    msgBox.setText("This action will delete all the data in the database.");
    msgBox.setInformativeText("Do you want to continue?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int answer = msgBox.exec();

    if(answer == QMessageBox::Ok)
    {
        db.deleteDb();
        db.init();

        if(db.getLastError().type() != QSqlError::NoError) {
            showError(db.getLastError());
            return;
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Database deleted.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();
        }
        checkDatabaseActions();
        tabSelected(ui->tabWidget->currentIndex()); // Reload information
    }
}

/*!
 * Trigger example data insertion to the database
 */
void MainWindow::initExampleDatabase()
{
    db.initExampleDatabase();
    if(db.getLastError().type() != QSqlError::NoError) {
        showError(db.getLastError());
        return;
    }

    QMessageBox msgBox;
    msgBox.setText("Example data inserted to the database");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();

    checkDatabaseActions();
    tabSelected(ui->tabWidget->currentIndex()); // Reload information
}

/*!
 * Import database from file
 *
 * Opens a db file given by user, deletes the database, copies the database file to the
 * default path and initializes the database class.
 */
void MainWindow::importDatabase()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Import database file"), QDir::rootPath(), tr("SQlite Database Files (*.db3)"));

    if(!fileName.isEmpty())
    {
        db.deleteDb();
        QFile::copy(fileName, db.getDbPath());
        db.init();

        QMessageBox msgBox;
        msgBox.setText("Database imported from:");
        msgBox.setInformativeText(fileName);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();

        checkDatabaseActions();
        tabSelected(ui->tabWidget->currentIndex()); // Reload information
    }
}

/*!
 * Export database to file
 */
void MainWindow::exportDatabase()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Export database file"), QDir::rootPath(), tr("SQlite Database Files (*.db3)"));

    if(!fileName.isEmpty())
    {
        QFile::copy(db.getDbPath(), fileName);

        QMessageBox msgBox;
        msgBox.setText("Database exported to:");
        msgBox.setInformativeText(fileName);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();

        checkDatabaseActions();
        tabSelected(ui->tabWidget->currentIndex()); // Reload information
    }
}

//_____GUI Slots_____
/*!
 * Triggers an action when a tab is selected. So far the transactions are loaded when transactionTab is loaded
 */
void MainWindow::tabSelected(int index)
{
    if(index==0)
    {
        if(ui->rbKittyTransactions->isChecked())
            ui->rbKittyTransactions->click();
        else if(ui->rbPersonalTransactions->isChecked())
            ui->rbPersonalTransactions->click();
        else if(ui->rbUsers->isChecked())
            ui->rbUsers->click();
        else if(ui->rbEvents->isChecked())
            ui->rbEvents->click();
    }
    else if(ui->tabWidget->currentIndex()==1)
        loadTransactions();
}

/*!
 * Shows the selected transaction on the table in the combo-boxes
 */
void MainWindow::selectTransaction(QModelIndex index)
{
    int row = index.row();
    QAbstractItemModel *model = ui->tvEventTransactions->model();
    int transactionId = model->data(model->index(row,0)).toInt();
    Transaction selectedTransaction = db.getTransaction(transactionId);

    selectIdInCmb(ui->cmbUserGives, selectedTransaction.getUserGiving().getId());
    selectIdInCmb(ui->cmbUserReceives, selectedTransaction.getUserReceiving().getId());
}

/*!
 * Downloads the gravatar corresponding to the email of the selected user
 */
void MainWindow::showUser(QModelIndex index)
{
    if(ui->rbUsers->isChecked())
    {
        int row = index.row();
        QAbstractItemModel *model = ui->tvTable->model();
        int userid = model->data(model->index(row,0)).toInt();
        User selectedUser = db.getUser(userid);

        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(gravatarDownloaded(QNetworkReply*)));
        manager->get(QNetworkRequest(QUrl(QString("https://www.gravatar.com/avatar/").append(selectedUser.getEmailHash()))));
    }
}

/*!
 * Shows the downloaded gravatar in a dialog
 */
void MainWindow::gravatarDownloaded(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
        return;
    }
    QVariant attribute = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (attribute.isValid()) {
        QUrl url = attribute.toUrl();
        qDebug() << "must go to:" << url;
        return;
    }
//    qDebug() << "ContentType:" << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);

    // Create dialog
    QDialog dialog(this);
    QHBoxLayout layout(&dialog);

    dialog.setWindowFlags(Qt::Dialog|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("User avatar");

    QLabel *image = new QLabel(&dialog);
    image->setPixmap(pixmap);
    layout.addWidget(image);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok,
                               Qt::Horizontal, &dialog);
    layout.addWidget(&buttonBox);

    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {

    }

    // Delete QNetwork Reply and Manager
    reply->abort();
    reply->deleteLater();
    reply->manager()->deleteLater();
}

//_____Helpers_____
/*!
 * Returns the database-id (column of the model) of the selected item of a combo-box to which a sql model has been assigned
 */
int MainWindow::getIdFromCmb(QComboBox *cmbBox)
{
    return cmbBox->model()->data(cmbBox->model()->index(cmbBox->currentIndex(),1)).toInt();
}

/*!
 * Changes the current database-index of the comboBox (with an sql model) to the one matching th given id
 */
void MainWindow::selectIdInCmb(QComboBox *cmbBox, int id)
{
    QAbstractItemModel *model = cmbBox->model();
    for(int i = 0; i < model->rowCount(); i++)
    {
        if(model->data(model->index(i,1)).toInt() == id)
        {
            cmbBox->setCurrentIndex(i);
            return;
        }
    }
}

/*!
 * Selects row of a tableView matching a given-id
 */
void MainWindow::selectRowInTable(QTableView *tableView, int id)
{
    QAbstractItemModel *model = tableView->model();
    for(int i = 0; i < model->rowCount(); i++)
    {
        if(model->data(model->index(i,0)).toInt() == id)
        {
            tableView->selectRow(i);
            return;
        }
    }
}

/*!
 * Selects row of a tableView matching a user giving and receiving pair
 */
void MainWindow::selectRowInTransactionTable(QTableView *tableView, QString userGives, QString userReceives)
{
    QAbstractItemModel *model = tableView->model();

    if(!model)
        return;

    for(int i = 0; i < model->rowCount(); i++)
    {
        if(model->data(model->index(i,1)).toString() == userGives && model->data(model->index(i,2)).toString() == userReceives)
        {
            tableView->selectRow(i);
            return;
        }
    }
}

//_____Database functions_____
/*!
 * Load users from database as entries of a combo-box
 */
bool MainWindow::loadUsersToCmb(QComboBox *cmbBox, bool includeKitty, QString condition)
{
    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    QString strQuery = "SELECT nickname, id FROM users";
    if(!includeKitty)
        strQuery.append(" WHERE users.id IS NOT " + QString::number(db.getKittyId()));
    if(!condition.isEmpty() && !includeKitty)
        strQuery.append(" AND " + condition);
    if(!condition.isEmpty() && includeKitty)
        strQuery.append(" WHERE " + condition);

    model->setQuery(strQuery);

    if(model->rowCount() != 0)
        cmbBox->setModel(model);

    return model->rowCount() == 0;
}

/*!
 * Load events from database as entries of a combo-box
 */
bool MainWindow::loadEventsToCmb(QComboBox *cmbBox, QString condition)
{
    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    QString strQuery = "SELECT name, id FROM events";
    if(!condition.isEmpty())
        strQuery.append(" WHERE " + condition);

    model->setQuery(strQuery);

    if(model->rowCount() != 0)
        cmbBox->setModel(model);

    return model->rowCount() == 0;
}

/*!
 * Load users from database to a table-view
 */
bool MainWindow::loadUsersToTable(QTableView *tableView, QString condition)
{
    // Create the data model
    globalModel = new QSqlRelationalTableModel(tableView);
    globalModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    globalModel->setTable("users");

    // Set the localized header captions
    globalModel->setHeaderData(globalModel->fieldIndex("name"), Qt::Horizontal, tr("User Name"));
    globalModel->setHeaderData(globalModel->fieldIndex("nickname"), Qt::Horizontal, tr("Nickname"));
    globalModel->setHeaderData(globalModel->fieldIndex("email"), Qt::Horizontal, tr("Email Address"));
    globalModel->setHeaderData(globalModel->fieldIndex("birthdate"), Qt::Horizontal, tr("Birthday Date"));

    QString filter = QString();
    filter = "id IS NOT " + QString::number(db.getKittyId());

    if(!condition.isEmpty())
    {
        filter.append(" AND ").append(condition);
    }
    globalModel->setFilter(filter);

    // Populate the model
    if (!globalModel->select()) {
        showError(globalModel->lastError());
        return true;
    }
    tableView->setModel(globalModel);
    tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));
    tableView->setColumnHidden(globalModel->fieldIndex("id"), true);
    tableView->setColumnHidden(globalModel->fieldIndex("passwordhash"), true);
    tableView->setColumnHidden(globalModel->fieldIndex("passwordsalt"), true);
    tableView->setCurrentIndex(globalModel->index(0, 0));

    return globalModel->rowCount() == 0;
}

/*!
 * Load events from database to a table-view
 */
bool MainWindow::loadEventsToTable(QTableView *tableView, QString condition)
{
    // Create the data model
    globalModel = new QSqlRelationalTableModel(tableView);
    globalModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    globalModel->setTable("events");

    // Set the relations to the other database tables
    globalModel->setRelation(globalModel->fieldIndex("admin"), QSqlRelation("users", "id", "nickname"));

    // Set the localized header captions
    globalModel->setHeaderData(globalModel->fieldIndex("name"), Qt::Horizontal, tr("Event Name"));
    globalModel->setHeaderData(globalModel->fieldIndex("creation"), Qt::Horizontal, tr("Creation Date"));
    globalModel->setHeaderData(globalModel->fieldIndex("place"), Qt::Horizontal, tr("Place"));
    globalModel->setHeaderData(globalModel->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    globalModel->setHeaderData(globalModel->fieldIndex("finished"), Qt::Horizontal, tr("Finished?"));
    globalModel->setHeaderData(globalModel->fieldIndex("admin"), Qt::Horizontal, tr("Administrator"));

    QString filter = QString();
    if(!condition.isEmpty())
    {
        filter = condition;
    }
    globalModel->setFilter(filter);

    // Populate the model
    if (!globalModel->select()) {
        showError(globalModel->lastError());
        return true;
    }
    tableView->setModel(globalModel);
    tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));
    tableView->setColumnHidden(globalModel->fieldIndex("id"), true);
    tableView->setCurrentIndex(globalModel->index(0, 0));

    return globalModel->rowCount() == 0;
}

/*!
 * Load transactions from database to a table-view
 */
bool MainWindow::loadTransactionsToTable(QTableView *tableView, bool showKitty, bool showPersonal, QString condition)
{
    // Create the data model
    globalModel = new QSqlRelationalTableModel(tableView);
    globalModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    globalModel->setTable("transactions");
    // Set the relations to the other database tables
    globalModel->setRelation(globalModel->fieldIndex("usergives"), QSqlRelation("users", "id", "nickname"));
    globalModel->setRelation(globalModel->fieldIndex("userreceives"), QSqlRelation("users", "id", "nickname"));
    globalModel->setRelation(globalModel->fieldIndex("event"), QSqlRelation("events", "id", "name"));
    // Set the localized header captions
    globalModel->setHeaderData(globalModel->fieldIndex("usergives"), Qt::Horizontal, tr("Giving user"));
    globalModel->setHeaderData(globalModel->fieldIndex("userreceives"), Qt::Horizontal, tr("Receiving user"));
    globalModel->setHeaderData(globalModel->fieldIndex("event"), Qt::Horizontal, tr("Event Name"));
    globalModel->setHeaderData(globalModel->fieldIndex("amount"), Qt::Horizontal, tr("Amount"));
    globalModel->setHeaderData(globalModel->fieldIndex("transactionDate"), Qt::Horizontal, tr("Transaction Date"));
    globalModel->setHeaderData(globalModel->fieldIndex("place"), Qt::Horizontal, tr("Place"));
    globalModel->setHeaderData(globalModel->fieldIndex("description"), Qt::Horizontal, tr("Description"));

    QString filter = QString();
    if(showKitty && !showPersonal)
    {
        filter = "userreceives = " + QString::number(db.getKittyId()) + " OR usergives = " + QString::number(db.getKittyId());
    }
    else if(!showKitty && showPersonal)
    {
        filter = "userreceives IS NOT " + QString::number(db.getKittyId()) +  " AND usergives IS NOT " + QString::number(db.getKittyId());
    }
    else if (!showKitty && !showPersonal)
    {
        filter.isEmpty();
    }

    if(!condition.isEmpty())
    {
        if(filter.isEmpty())
            filter = condition;
        else
            filter.append(" AND (" + condition + ")");
    }
    globalModel->setFilter(filter);

    // Populate the model
    if (!globalModel->select()) {
        showError(globalModel->lastError());
        return true;
    }
    tableView->setModel(globalModel);
    tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));
    tableView->setColumnHidden(globalModel->fieldIndex("id"), true);
    tableView->setCurrentIndex(globalModel->index(0, 0));

    return globalModel->rowCount() == 0;
}
