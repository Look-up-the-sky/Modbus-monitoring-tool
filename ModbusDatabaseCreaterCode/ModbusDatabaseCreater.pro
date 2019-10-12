#-------------------------------------------------
#
# Project created by QtCreator 2019-09-03T13:51:54
#
#-------------------------------------------------

QT       += core gui sql
TRANSLATIONS += C_I18N_en_UK.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModbusDatabaseCreater
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    logindialog.cpp \
    projectcreatdialog.cpp \
    registerdialog.cpp \
    addsignaldialog.cpp \
    addscenezonedialog.cpp \
    addsceneitemdialog.cpp \
    scenemgrdialog.cpp

HEADERS += \
        mainwindow.h \
    logindialog.h \
    projectcreatdialog.h \
    registerdialog.h \
    addsignaldialog.h \
    addscenezonedialog.h \
    addsceneitemdialog.h \
    scenemgrdialog.h

FORMS += \
        mainwindow.ui \
    logindialog.ui \
    projectcreatdialog.ui \
    registerdialog.ui \
    addsignaldialog.ui \
    addscenezonedialog.ui \
    addsceneitemdialog.ui \
    scenemgrdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    language.qrc
