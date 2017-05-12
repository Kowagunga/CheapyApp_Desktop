#include "dbclasses.h"
#include <QCryptographicHash>

/*!
 * Empty User constructor
 */
User::User()
{
    this->id = -1;
    this->name = QString();
    this->nickname = QString();
    this->email = QString();
    this->passwordHash = QString();
    this->passwordSalt = QString();
    this->birthdate = QDate();
}

/*!
 * User constructor that requires id.
 *
 * The other parameters can be given, but are not required.
 */
User::User(int id, QString name, QString nickname, QString email, QString pwdHash, QString pwdSalt, QDate birthdate)
{
    this->id = id;
    this->name = name;
    this->nickname = nickname;
    this->email = email;
    this->passwordHash = pwdHash;
    this->passwordSalt = pwdSalt;
    this->birthdate = birthdate;
}

/*!
 * User constructor with password hash and salt and without an id.
 *
 * All parameters are required. After inserting in the database, the
 * resulting id can be set with \sa User::setId(int id)
 */
User::User(QString name, QString nickname, QString email, QString pwdHash, QString pwdsalt, QDate birthdate)
{
    this->id = -1;
    this->name = name;
    this->nickname = nickname;
    this->email = email;
    this->passwordHash = pwdHash;
    this->passwordSalt = pwdsalt;
    this->birthdate = birthdate;
}

/*!
 * User constructor with plain password and without an id
 *
 * All parameters are required. Password hash is generated. After inserting in the
 * database, the resulting id can be set with \sa User::setId(int id)
 */
User::User(QString name, QString nickname, QString email, QString password, QDate birthdate)
{
    this->id = -1;
    this->name = name;
    this->nickname = nickname;
    this->email = email;
    this->birthdate = birthdate;
    hashPassword(password);
}

/*!
 * Validates an email address
 *
 * Checks if it's a valid email address and if it has an intermediate address
 */
bool User::validateEmail()
{
    const QRegExp m_validMailRegExp = QRegExp("[a-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,4}");
    const QRegExp m_intermediateMailRegExp = QRegExp("[a-z0-9._%+-]*@?[a-z0-9.-]*\\.?[a-z]*");

    QString text = email.trimmed().toLower();

    if (m_validMailRegExp.exactMatch(text))
        return true;
    if (m_intermediateMailRegExp.exactMatch(text))
        return false;

    return false;
}

/*!
 * Stores the hash of the given password
 *
 * Generates a random salt and stores the salt and the hash of the concatenated password with that salt.
 */
void User::hashPassword(QString password)
{
    QString salt = generatePwdSalt();
    QString hash = QString(QCryptographicHash::hash(password.append(salt).toUtf8(),QCryptographicHash::Sha256).toHex());
    this->passwordHash = hash;
    this->passwordSalt = salt;
}

/*!
 * Checks if the received string matches the password user
 *
 * Generates the hash of the received password with the user salt and compares it
 * against the user password hash. Returns true if correct.
 */
bool User::checkPassword(QString password)
{
    if(passwordHash.isEmpty() || passwordSalt.isEmpty() || password.isEmpty())
        return false;

    QString hash = QString(QCryptographicHash::hash(password.append(this->passwordSalt).toUtf8(),QCryptographicHash::Sha256).toHex());

    if(QString::compare(hash,passwordHash,Qt::CaseSensitive) == 0)
        return true;
    else
        return false;
}

/*!
 * Returns the current age of the user in complete years
 */
int User::getAge()
{
    QDate currentDate = QDate::currentDate();
    int currentAge = currentDate.year() - birthdate.year();
    if (birthdate.month() > currentDate.month()
         ||
         (birthdate.month()==currentDate.month()
          &&
          birthdate.day()>currentDate.day()))
    {
        currentAge--;
    }

    return currentAge;
}

/*!
 * Generates a 16 character string out of random digits, low- and uppercase letters
 */
QString User::generatePwdSalt()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 16;

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

/*!
 * Empty Event constructor
 */
Event::Event()
{
    this->id = -1;
    this->name = QString();
    this->creationDate = QDate();
    this->admin = User();
    this->place = QString();
    this->description = QString();
    this->finished = false;
}

/*!
 * Event constructor that requires id.
 *
 * The other parameters can be given, but are not required.
 */
Event::Event(int id, QString name, QDate creation, User admin, QString place, QString description, bool finished)
{
    this->id = id;
    this->name = name;
    this->creationDate = creation;
    this->admin = admin;
    this->place = place;
    this->description = description;
    this->finished = finished;
}

/*!
 * Event constructor without an id.
 *
 * All parameters are required. After inserting in the database, the
 * resulting id can be set with \sa Event::setId(int id)
 */
Event::Event(QString name, QDate creation, User admin, QString place, QString description, bool finished)
{
    this->id = -1;
    this->name = name;
    this->creationDate = creation;
    this->admin = admin;
    this->place = place;
    this->description = description;
    this->finished = finished;
}

/*!
 * Empty Transaction constructor
 */
Transaction::Transaction()
{
    this->id = -1;
    this->userGiving = User();
    this->userReceiving = User();
    this->event = Event();
    this->amount = 0;
    this->date = QDate();
    this->place = QString();
    this->description = QString();
}

/*!
 * Transaction constructor that requires id.
 *
 * The other parameters can be given, but are not required.
 */
Transaction::Transaction(int id, User userGiving, User userReceiving, Event event, double amount, QDate date, QString place, QString description)
{
    this->id = id;
    this->userGiving = userGiving;
    this->userReceiving = userReceiving;
    this->event = event;
    this->amount = amount;
    this->date = date;
    this->place = place;
    this->description = description;
}

/*!
 * Transaction constructor without an id.
 *
 * All parameters are required. After inserting in the database, the
 * resulting id can be set with \sa Transaction::setId(int id)
 */
Transaction::Transaction(User userGiving, User userReceiving, Event event, double amount, QDate date, QString place, QString description)
{
    this->id = -1;
    this->userGiving = userGiving;
    this->userReceiving = userReceiving;
    this->event = event;
    this->amount = amount;
    this->date = date;
    this->place = place;
    this->description = description;
}
