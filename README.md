# CheapyApp_Desktop {#mainpage}
Desktop software written in C++/Qt for managing economic transactions among users for events.

## Version history
* Version 0.1.1
    * Classes for each table in the database (User, Event, Transaction)
	* MainWindow and DataBase uses this classes (OOD)
	* Documentation with doxygen (26/03/2017)

## General to-do list
\todo
* Database
    *  Make database exportable
    *  Import database if file exists, otherwise initialize tables
*  GUI
    *  Current GUI should be converted to an administrator view or event overview
    *  Initial view should be (login ->) events for an user. Maybe ongoing event as default
    *  Event default view should show event basic information, add transaction view, conclude event view