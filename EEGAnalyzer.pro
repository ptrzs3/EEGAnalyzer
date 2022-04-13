#-------------------------------------------------
#
# Project created by QtCreator 2022-03-26T19:20:56
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += sql
QT       += multimedia
QT       += multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EEGAnalyzer
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

DEFINES += QT_DLL QWT_DLL

#LIBS += -L"C:\Qt\Qt5.12.2\5.12.2\mingw73_32\lib" -lqwtd

LIBS += -L"C:\Qt\Qt5.12.2\5.12.2\mingw73_32\lib" -lqwt

INCLUDEPATH += C:\Qt\Qt5.12.2\5.12.2\mingw73_32\include\Qwt
RC_ICONS=brain.ico
SOURCES += \
        main.cpp \
    dynplot.cpp \
    timescaledraw.cpp \
    videoplayer.cpp \
    database.cpp \
    manager.cpp \
    BarChart.cpp \
    staplot.cpp \
    tgamparser.cpp

HEADERS += \
    identifiers.h \
    dynplot.h \
    timescaledraw.h \
    videoplayer.h \
    database.h \
    BarChart.h \
    manager.h \
    staplot.h \
    tgamparser.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
