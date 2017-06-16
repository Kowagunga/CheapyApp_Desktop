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
    // initialize the database
    if(db.getLastError().type() != QSqlError::NoError) {
        qDebug() << db.getLastError();
        return;
    }
}

cheapyapptest::~cheapyapptest()
{

}

void cheapyapptest::initTestCase()
{

}

void cheapyapptest::cleanupTestCase()
{

}

void cheapyapptest::exportDatabase()
{

}

void cheapyapptest::deleteDatabase()
{

}

void cheapyapptest::initializeDatabase()
{

}

void cheapyapptest::importDatabase()
{

}

QTEST_APPLESS_MAIN(cheapyapptest)

#include "tst_cheapyapptest.moc"
