#include <QtTest>

#include "../CheapyApp_Source/database.h"

class cheapyapptest : public QObject
{
    Q_OBJECT

public:
    cheapyapptest();
    ~cheapyapptest();

private:
    DataBase db;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void exportDatabase();
    void deleteDatabase();
    void initializeDatabase();
    void importDatabase();

};

cheapyapptest::cheapyapptest()
{

}

cheapyapptest::~cheapyapptest()
{

}

void cheapyapptest::initTestCase()
{
    db.init();

    QVERIFY(db.getLastError().type() == QSqlError::NoError);
}

void cheapyapptest::cleanupTestCase()
{
    QString fileName = db.getDbPath();
    bool success = QFile::remove(fileName.append(".bak"));

    QVERIFY(success);
}

void cheapyapptest::exportDatabase()
{
    QString fileName = db.getDbPath();
    bool success = QFile::copy(fileName, fileName.append(".bak"));

    QVERIFY(success);
}

void cheapyapptest::deleteDatabase()
{
    bool success  = db.deleteDb();

    QVERIFY(success);
}

void cheapyapptest::initializeDatabase()
{
    db.initExampleDatabase();

    QVERIFY(db.getLastError().type() == QSqlError::NoError);
}

void cheapyapptest::importDatabase()
{
    bool success  = db.deleteDb();

    QString fileName = db.getDbPath();
    success &= QFile::copy(fileName.append(".bak"), db.getDbPath());

    db.init();
    success &= db.getLastError().type() == QSqlError::NoError;

    QVERIFY(success);
}

QTEST_APPLESS_MAIN(cheapyapptest)

#include "tst_cheapyapptest.moc"
