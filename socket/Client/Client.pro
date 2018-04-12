#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T18:01:30
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
        tcpclient.cpp \
    chatwindow.cpp \
    p2pdialog.cpp \
    regdialog.cpp \
    filerecdialog.cpp

HEADERS  += tcpclient.h \
    chatwindow.h \
    p2pdialog.h \
    regdialog.h \
    filerecdialog.h

FORMS    += tcpclient.ui \
    chatwindow.ui \
    p2pdialog.ui \
    regdialog.ui \
    filerecdialog.ui
