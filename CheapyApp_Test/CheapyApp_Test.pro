QT += testlib core sql
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += tst_cheapyapptest.cpp \
    ../CheapyApp_Source/database.cpp \
    ../CheapyApp_Source/dbclasses.cpp

HEADERS += ../CheapyApp_Source/database.h \
    ../CheapyApp_Source/dbclasses.h

