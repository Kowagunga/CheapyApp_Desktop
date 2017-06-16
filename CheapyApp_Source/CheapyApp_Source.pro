#-------------------------------------------------
#
# Project created by QtCreator 2016-09-13T11:31:56
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CheapyApp_Desktop
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    database.cpp \
    dbclasses.cpp

HEADERS  += mainwindow.h \
    database.h \
    dbclasses.h

FORMS    += mainwindow.ui

#Application version
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_BUILD = 1

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"
win32 {
DEFINES += BUILDTIME=\\\"$$system('echo %time%')\\\"
DEFINES += BUILDDATE=\\\"$$system('echo %date%')\\\"
} else {
DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M.%s')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%d/%m/%y')\\\"
}

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}
