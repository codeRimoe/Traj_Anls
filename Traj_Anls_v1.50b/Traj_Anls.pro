#-------------------------------------------------
#
# Project created by QtCreator 2018-10-23T13:24:11
#
#-------------------------------------------------

QT       += core gui opengl
INCLUDEPATH += $$PWD
LIBS += -lOpengl32 -lglu32 -lglut

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Traj_Anls
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    setting.cpp \
    CBase.cpp \
    CDataSet.cpp \
    CLocation.cpp \
    CStay.cpp \
    CTraj.cpp \
    display.cpp \
    oglwidget.cpp \
    save.cpp

HEADERS += \
        mainwindow.h \
    setting.h \
    CBase.h \
    CDataSet.h \
    CLocation.h \
    CStay.h \
    CTraj.h \
    display.h \
    oglwidget.h \
    save.h

FORMS += \
        mainwindow.ui \
    setting.ui \
    display.ui \
    save.ui

RESOURCES += \
    resources.qrc



win32: LIBS += -LC:/gdal32/lib/ -lgdal_i

INCLUDEPATH += C:/gdal32/include
DEPENDPATH += C:/gdal32/include

RC_FILE = una.rc