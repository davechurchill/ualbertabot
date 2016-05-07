#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T14:07:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BOSSGUI
TEMPLATE = app

INCLUDEPATH += C:\\libraries\\BWAPI_412\\include\\
INCLUDEPATH += d:\\ualbertabot\\BOSS\\source\\

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:RC_ICONS += Protoss_Probe.ico

LIBS += -L$$PWD/../bin/ -lBOSS
LIBS += -LC:/libraries/BWAPI_412/lib/ -lBWAPI

DESTDIR = $$PWD/../../bin/
