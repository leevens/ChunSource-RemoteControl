#-------------------------------------------------
#
# Project created by QtCreator 2018-12-16T11:58:04
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemotControlServer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES  -= UNICODE
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    server.cpp \
    thread.cpp \
    file.cpp \
    screen.cpp \ 
    keyboard.cpp \
    system.cpp \
    shell.cpp
HEADERS += \
        mainwindow.h \
    server.h \
    thread.h \
    file.h \
    screen.h \ 
    keyboard.h \
    system.h \
    shell.h

FORMS += \
        mainwindow.ui \
    file.ui \
    screen.ui \
    keyboard.ui \
    system.ui \
    shell.ui

RESOURCES += \
    resource.qrc
