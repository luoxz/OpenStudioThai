#-------------------------------------------------
#
# Project created by QtCreator 2015-06-01T17:57:53
#
#-------------------------------------------------

QT += core gui xml
QT += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ReportCompare
TEMPLATE = app

MOC_DIR = .

SOURCES += main.cpp\
            dropfilebutton.cpp \
            genbecreport.cpp \
            comparedialog.cpp \
            menuwindow.cpp

HEADERS  += \
    dropfilebutton.h \
    genbecreport.h \
    comparedialog.h \
    menuwindow.h

FORMS    += \
    comparedialog.ui \
    menuwindow.ui
