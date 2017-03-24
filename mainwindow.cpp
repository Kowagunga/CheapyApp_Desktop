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

    this->setWindowTitle("CheapyApp");

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

    connect(ui->tvEventTransactions, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectTransaction(QModelIndex)));
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
//    qDebug() << "MainWindow::ShowTable() - " << "Sender:" << sender()->objectName();

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
        showError(model->lastError());

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
        showError(model->lastError());

    bool oldState = ui->cmbUserGives->blockSignals(true);
    ui->cmbUserGives->setModel(model);
    ui->cmbUserGives->blockSignals(oldState);

    loadTransactionsToTable(ui->tvEventTransactions, true, true, QString("event = " + QString::number(eventId)));

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
        showError(model->lastError());

    bool oldState = ui->cmbUserReceives->blockSignals(true);
    ui->cmbUserReceives->setModel(model);
    ui->cmbUserReceives->blockSignals(oldState);

    updateTransactionAmount();
}

/*!
 * Shows the amount of money given from user giving to user receiving minus the amount from user receiving
 * to user giving.
 * \todo Amounts should be only positive
 */
void MainWindow::updateTransactionAmount()
{
    int eventId = getIdFromCmb(ui->cmbEvent);
    int userGivingId = getIdFromCmb(ui->cmbUserGives);
    int userReceivingId = getIdFromCmb(ui->cmbUserReceives);

    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery("SELECT SUM(amount) FROM transactions WHERE transactions.event = " + QString::number(eventId) +
                               " AND transactions.usergives = " + QString::number(userGivingId) +
                               " AND transactions.userreceives = " + QString::number(userReceivingId));

    if(model->rowCount() == 0)
        showError(model->lastError());

    double value = model->data(model->index(0,0)).toDouble();

    // Create the data model
    model = new QSqlQueryModel;
    model->setQuery("SELECT SUM(amount) FROM transactions WHERE transactions.event = " + QString::number(eventId) +
                               " AND transactions.usergives = " + QString::number(userReceivingId) +
                               " AND transactions.userreceives = " + QString::number(userGivingId));

    if(model->rowCount() == 0)
        showError(model->lastError());

    value -= model->data(model->index(0,0)).toDouble();

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
        // todo: check dialog before closing it
        QString problem;
        if(leName->text().isEmpty())
            problem = "The field 'Name' cannot be empty";
        else if(leNickname->text().isEmpty())
            problem = "The field 'Nickname' cannot be empty";

        if(problem == "")
        {
            // save User
            QSqlQuery query;
            query.prepare(db.getInsertUserQuery());
            db.addUser(query,User(leName->text(),leNickname->text(),deBirthday->date()));
            if(ui->rbUsers->isChecked())
                ui->rbUsers->click();
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
    QDateEdit *deStart = new QDateEdit(&dialog);
    deStart->setDate(QDateTime::currentDateTime().date());
    deStart->setDisplayFormat("dd.MM.yyyy");
    form.addRow("Start:", deStart);
    QDateEdit *deEnd = new QDateEdit(&dialog);
    deEnd->setDate(QDateTime::currentDateTime().date());
    deEnd->setDisplayFormat("dd.MM.yyyy");
    form.addRow("End:", deEnd);
    QLineEdit *lePlace = new QLineEdit(&dialog);
    lePlace->setMaxLength(30);
    form.addRow("Place:", lePlace);
    QLineEdit *leDescription = new QLineEdit(&dialog);
    leDescription->setMaxLength(50);
    form.addRow("Description:", leDescription);
    QComboBox *cmbAdmin = new QComboBox(&dialog);
    loadUsersToCmb(cmbAdmin,false,"");
    form.addRow("Admin:", cmbAdmin);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        // todo: check dialog before closing it
        QString problem;
        if(leName->text().isEmpty())
            problem = "The field 'Name' cannot be empty";
        else if (deEnd->date() < deStart->date())
            problem = "The end date must be the same or later than the start date";

        if(problem == "")
        {
            // save Event
            QSqlQuery query;
            query.prepare(db.getInsertEventQuery());
            db.addEvent(query, Event(leName->text(), deStart->date(), deEnd->date(), User(getIdFromCmb(cmbAdmin)), lePlace->text(), leDescription->text(), 0));
            if(ui->rbEvents->isChecked())
                ui->rbEvents->click();
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
    loadEventsToCmb(cmbEvents,"");
    form.addRow("Event:", cmbEvents);
    QComboBox *cmbUserGives = new QComboBox(&dialog);
    loadUsersToCmb(cmbUserGives,true,"");
    form.addRow("User giving:", cmbUserGives);
    QComboBox *cmbUserReceives = new QComboBox(&dialog);
    loadUsersToCmb(cmbUserReceives,true,"");
    form.addRow("User receiving:", cmbUserReceives);
    QDoubleSpinBox *dsbAmount = new QDoubleSpinBox(&dialog);
    form.addRow("Amount:", dsbAmount);
    dsbAmount->setDecimals(2);
    dsbAmount->setMinimum(-100000.00);
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


    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        // todo: check dialog before closing it
        QString problem;
        if(getIdFromCmb(cmbUserGives)==getIdFromCmb(cmbUserReceives))
            problem = "User giving must be different from user receiving";
        // todo: date between start and end date from event

        if(problem == "")
        {
            // save Transaction
            QSqlQuery query;
            query.prepare(db.getInsertTransactionQuery());
            db.addTransaction(query, Transaction(User(getIdFromCmb(cmbUserGives)), User(getIdFromCmb(cmbUserReceives)), Event(getIdFromCmb(cmbEvents)),
                           dsbAmount->value(), deTransactionDate->date(), lePlace->text(), leDescription->text()));
            if(ui->rbKittyTransactions->isChecked())
                ui->rbKittyTransactions->click();
            if(ui->rbPersonalTransactions->isChecked())
                ui->rbPersonalTransactions->click();
            if(ui->tabWidget->currentIndex()==1)
                loadTransactions();
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
    loadUsersToCmb(cmbUser,false,"");
    form.addRow("User:", cmbUser);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {
        int transactions, events;
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
        else
        {
            db.deleteUser(getIdFromCmb(cmbUser));
            if(ui->rbUsers->isChecked())
                ui->rbUsers->click();
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
    loadEventsToCmb(cmbEvent,"");
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
            if(ui->rbUsers->isChecked())
                ui->rbUsers->click();
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
    loadTransactionsToTable(tvTransactions, true, true);
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
        if(ui->rbKittyTransactions->isChecked())
            ui->rbKittyTransactions->click();
        if(ui->rbPersonalTransactions->isChecked())
            ui->rbPersonalTransactions->click();
        if(ui->tabWidget->currentIndex()==1)
            loadTransactions();
    }
}

//_____GUI Slots_____
/*!
 * Triggers an action when a tab is selected. So far the transactions are loaded when transactionTab is loaded
 */
void MainWindow::tabSelected(int index)
{
    if(index == 1)
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
void MainWindow::loadUsersToCmb(QComboBox *cmbBox, bool includeKitty, QString condition)
{
    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    QString strQuery = "SELECT nickname, id FROM users";
    if(!includeKitty)
        strQuery.append(" WHERE users.id IS NOT " + QString::number(db.getKittyId()));
    if(condition!="" && !includeKitty)
        strQuery.append(" AND " + condition);
    if(condition!="" && includeKitty)
        strQuery.append(" WHERE " + condition);

    model->setQuery(strQuery);

    if(model->rowCount() == 0)
        showError(model->lastError());

    cmbBox->setModel(model);
}

/*!
 * Load events from database as entries of a combo-box
 */
void MainWindow::loadEventsToCmb(QComboBox *cmbBox, QString condition)
{
    // Create the data model
    QSqlQueryModel *model = new QSqlQueryModel;
    QString strQuery = "SELECT name, id FROM events";
    if(condition!="")
        strQuery.append(" WHERE " + condition);

    model->setQuery(strQuery);

    if(model->rowCount() == 0)
        showError(model->lastError());

    cmbBox->setModel(model);
}

/*!
 * Load users from database to a table-view
 */
void MainWindow::loadUsersToTable(QTableView *tableView, QString condition)
{
    // Create the data model
    globalModel = new QSqlRelationalTableModel(tableView);
    globalModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    globalModel->setTable("users");

    // Set the localized header captions
    globalModel->setHeaderData(globalModel->fieldIndex("name"), Qt::Horizontal, tr("User Name"));
    globalModel->setHeaderData(globalModel->fieldIndex("nickname"), Qt::Horizontal, tr("Nickname"));
    globalModel->setHeaderData(globalModel->fieldIndex("birthdate"), Qt::Horizontal, tr("Birthday Date"));

    QString filter;
    filter = "id IS NOT " + QString::number(db.getKittyId());

    if(condition != "")
    {
        filter.append(" AND ").append(condition);
    }
    globalModel->setFilter(filter);

    // Populate the model
    if (!globalModel->select()) {
        showError(globalModel->lastError());
        return;
    }
    tableView->setModel(globalModel);
    tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));
    tableView->setColumnHidden(globalModel->fieldIndex("id"), true);
    tableView->setCurrentIndex(globalModel->index(0, 0));
}

/*!
 * Load events from database to a table-view
 */
void MainWindow::loadEventsToTable(QTableView *tableView, QString condition)
{
    // Create the data model
    globalModel = new QSqlRelationalTableModel(tableView);
    globalModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    globalModel->setTable("events");

    // Set the relations to the other database tables
    globalModel->setRelation(globalModel->fieldIndex("admin"), QSqlRelation("users", "id", "nickname"));

    // Set the localized header captions
    globalModel->setHeaderData(globalModel->fieldIndex("name"), Qt::Horizontal, tr("Event Name"));
    globalModel->setHeaderData(globalModel->fieldIndex("start"), Qt::Horizontal, tr("Start Date"));
    globalModel->setHeaderData(globalModel->fieldIndex("end"), Qt::Horizontal, tr("End Date"));
    globalModel->setHeaderData(globalModel->fieldIndex("place"), Qt::Horizontal, tr("Place"));
    globalModel->setHeaderData(globalModel->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    globalModel->setHeaderData(globalModel->fieldIndex("finished"), Qt::Horizontal, tr("Finished?"));
    globalModel->setHeaderData(globalModel->fieldIndex("admin"), Qt::Horizontal, tr("Administrator"));

    QString filter;
    if(condition != "")
    {
        filter = condition;
    }
    globalModel->setFilter(filter);

    // Populate the model
    if (!globalModel->select()) {
        showError(globalModel->lastError());
        return;
    }
    tableView->setModel(globalModel);
    tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));
    tableView->setColumnHidden(globalModel->fieldIndex("id"), true);
    tableView->setCurrentIndex(globalModel->index(0, 0));
}

/*!
 * Load transactions from database to a table-view
 */
void MainWindow::loadTransactionsToTable(QTableView *tableView, bool showKitty, bool showPersonal, QString condition)
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

    QString filter;
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
        filter = "";
    }

    if(condition != "")
    {
        if(filter == "")
            filter = condition;
        else
            filter.append(" AND (" + condition + ")");
    }
    globalModel->setFilter(filter);

    // Populate the model
    if (!globalModel->select()) {
        showError(globalModel->lastError());
        return;
    }
    tableView->setModel(globalModel);
    tableView->setItemDelegate(new QSqlRelationalDelegate(tableView));
    tableView->setColumnHidden(globalModel->fieldIndex("id"), true);
    tableView->setCurrentIndex(globalModel->index(0, 0));
}
