# CheapyApp_Desktop {#mainpage}
Desktop software written in C++/Qt for managing economic transactions among users for events.

## Version history
* Version 0.1.1
    * Classes for each table in the database (User, Event, Transaction)
	* MainWindow and DataBase use these classes (OOD)
	* Documentation with doxygen (26/03/2017)
        * Password field for users. Stored as SHA256 hash and 16 character salt (11/05/2017)
        * Email field for users and email validator. Event has creation date instad of start/end. (12/05/2017)

## General to-do list
\todo
*  Database
    *  Place database file in a better path
*  GUI
    *  Current GUI should be converted to an administrator view or event overview
    *  Initial view should be (login ->) events for an user. Maybe ongoing event as default
    *  Event default view should show event basic information, add transaction view, conclude event view
    *  Use QStackedWidget to change between UIs
*  Test
    *  Create automatized tests using QTestLib (or Squish free trial)
