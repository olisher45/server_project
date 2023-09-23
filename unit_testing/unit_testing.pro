QT += testlib
QT -= gui

QT += core
QT += network
QT += sql

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_server_test.cpp

SOURCES += ../NewServer/database.cpp \
    ../NewServer/server.cpp \
    ../NewServer/tcpserver.cpp \
    ../NewServer/taskmanager.cpp

HEADERS += ../NewServer/database.h \
    ../NewServer/server.h \
    ../NewServer/tcpserver.h \
    ../NewServer/taskmanager.h
