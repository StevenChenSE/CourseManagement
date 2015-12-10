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
    entrance.cpp \
    teacherwindow.cpp \
    adminwindow.cpp \
    initcoursedialog.cpp \
    studentwindow.cpp

HEADERS  += \
    logindialog.h \
    entrance.h \
    teacherwindow.h \
    adminwindow.h \
    qtheaders.h \
    initcoursedialog.h \
    studentwindow.h

FORMS    += \
    logindialog.ui \
    entrance.ui \
    teacherwindow.ui \
    adminwindow.ui \
    initcoursedialog.ui \
    studentwindow.ui

DISTFILES += \
    init_sql.sql \
    data_insert.sql
PRECOMPILED_HEADER += qtheaders.h
