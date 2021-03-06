#-------------------------------------------------
#
# Project created by QtCreator 2013-12-12T17:59:22
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plist_rw
TEMPLATE = app


SOURCES += ../../src/ui/imagelistwidget.cpp \
    ../../src/ui/mainwindow.cpp \
    ../../src/ui/workplacewidget.cpp \
    ../../src/main.cpp \
    ../../src/core/framecollector.cpp \
    ../../src/ui/attributewidget.cpp \
    ../../src/io/pugixml.cpp \
    ../../src/io/plistxmlreader.cpp \
    ../../src/io/plistxmlwriter.cpp \
    ../../src/core/alignalgorithm.cpp \
    ../../src/ui/settingdialog.cpp

HEADERS  += ../../src/ui/imagelistwidget.h \
    ../../src/ui/mainwindow.h \
    ../../src/ui/workplacewidget.h \
    ../../src/ui/attributewidget.h \
    ../../include/io/base64.hpp \
    ../../include/io/Plist.hpp \
    ../../include/io/PlistDate.hpp \
    ../../include/io/pugiconfig.hpp \
    ../../include/io/pugixml.hpp \
     ../../include/io/plistxmlreader.h \
    ../../include/io/plistxmlwriter.h \
    ../../src/util/util.h \
    ../../include/core/alignalgorithm.h \
    ../../include/core/frame.h \
    ../../include/core/framecollector.h \
    ../../src/ui/settingdialog.h

FORMS    += ../../res/mainwindow.ui \
    ../../res/settingdialog.ui

INCLUDEPATH += ../../include/
INCLUDEPATH += ../../3dpart/
INCLUDEPATH += ../../src/

