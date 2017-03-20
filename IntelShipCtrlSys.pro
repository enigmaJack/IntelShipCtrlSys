#-------------------------------------------------
#
# Project created by QtCreator 2017-02-19T10:30:04
#
#-------------------------------------------------
include(./com/qextserialport.pri)

QT       += core gui

TARGET = IntelShipCtrlSys
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += "/usr/local/include/njnav/"
INCLUDEPATH += "/usr/local/include/boost/"
INCLUDEPATH += "/usr/local/include/"
INCLUDEPATH += "/usr/local/include/eigen3/"
INCLUDEPATH += "/opt/nros/sdk/sdk/include/"

LIBS += -L /usr/local/lib -lboost_system
LIBS += librplidar_sdk.a libnjnav_foundation.a libnjnav_localization.a

SOURCES += main.cpp \
    com/command.cpp \
    slamtec/slamtec.cpp \
    test.cpp \
    display/diagramscene.cpp \
    display/diagramCurnve.cpp

HEADERS += \
    com/command.h \
    slamtec/slamtec.h \
    test.h \
    display/diagramscene.h \
    display/diagramCurnve.h

FORMS += \
    test.ui
