#-------------------------------------------------
#
# Project created by QtCreator 2015-11-29T21:19:16
#
#-------------------------------------------------

QT       += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CourseManagement
TEMPLATE = app


SOURCES += main.cpp\
    logindialog.cpp \
    entrance.cpp

HEADERS  += \
    logindialog.h \
    entrance.h

FORMS    += \
    logindialog.ui \
    entrance.ui

DISTFILES += \
    init_sql.sql \
    data_insert.sql
