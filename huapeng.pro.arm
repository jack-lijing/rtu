#-------------------------------------------------
#
# Project created by QtCreator 2019-01-22T14:24:02
#
#-------------------------------------------------

QT       += core gui sql concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = huapeng
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

SOURCES += main.cpp mainwindow.cpp  canshubiao.cpp\
    QtSoftKeyboard/frminput.cpp \
    QtSoftKeyboard/frmmain.cpp \
    QtSoftKeyboard/frmnum.cpp \
    ./core/dosqlite.c \
    ./core/protocol.c \
    ./core/csapp.c \
    shezhi.cpp \
    ./core/uartconfig.c \
    dspthread.cpp \
    mythread.cpp \
    zhuangtaibiao.cpp \
    dtuthread.cpp



HEADERS += \
        mainwindow.h \
    canshubiao.h \
    shezhi.h \
    QtSoftKeyboard/frminput.h \
    QtSoftKeyboard/frmmain.h \
    QtSoftKeyboard/frmnum.h \
    ./core/csapp.h \
    ./core/rtu.h \
    ./core/uartconfig.h \
    ./core/sqlite3.h \
    dspthread.h \
    mythread.h \
    zhuangtaibiao.h \
    dtuthread.h

FORMS += \
        mainwindow.ui \
    canshubiao.ui \
    ./QtSoftKeyboard/frminput.ui \
    ./QtSoftKeyboard/frmmain.ui \
    ./QtSoftKeyboard/frmnum.ui \
    shezhi.ui \
    zhuangtaibiao.ui

INCLUDEPATH += ./core/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=


unix:!macx: LIBS += -L$$PWD/../lib/arm/ -lcrc

INCLUDEPATH += $$PWD/../lib/arm
DEPENDPATH += $$PWD/../lib/arm

unix:!macx: PRE_TARGETDEPS += $$PWD/../lib/arm/libcrc.a

unix:!macx: LIBS += -L$$PWD/../lib/arm/ -lsqlite3

INCLUDEPATH += $$PWD/../lib/arm
DEPENDPATH += $$PWD/../lib/arm

unix:!macx: PRE_TARGETDEPS += $$PWD/../lib/arm/libsqlite3.a
