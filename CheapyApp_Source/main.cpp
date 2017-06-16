#include "mainwindow.h"
#include <QApplication>

/*!
 * \brief Main function of the application
 * \param argc number of input arguments
 * \param argv text of input arguments
 * \return result of the application
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
