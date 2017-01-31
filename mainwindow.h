#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtSql>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
private slots:
    void showTable();
    void updateTransactionUserGiving();
    void updateTransactionUserReceiving();
    void updateTransactionAmount();
    void newEvent();
    void newUser();
    void newTransaction();
    void deleteUser();
    void deleteEvent();
    void deleteTransaction();
    void tabSelected(int index);

private:
    Ui::MainWindow *ui;

    void showError(const QSqlError &err);
    QSqlRelationalTableModel *model;
    QSqlQueryModel *transactionModel;
    void loadTransactions();
    int getIdFromQCombobox(QComboBox *cmbBox);
    void loadUsersToCmb(QComboBox *cmbBox, bool includeKitty, QString condition);
    void loadEventsToCmb(QComboBox *cmbBox, QString condition);
    void loadTransactionsToTable(QTableView *tableView, bool showKitty = true, bool showPersonal = true, QString condition = "");
    void loadUsersToTable(QTableView *tableView, QString condition = "");
    void loadEventsToTable(QTableView *tableView, QString condition = "");
};

#endif // MAINWINDOW_H
