SOURCES += main.cpp ../../core/tag.cpp dialog.cpp
HEADERS += ../../core/tag.h dialog.h
FORMS  += projman.ui

TARGET = qtapp
TEMPLATE = lib
QMAKE_CXXFLAGS += -std=gnu++0x
MOC_DIR     = ../../../obj/qt/app/moc/
OBJECTS_DIR = ../../../obj/qt/app/obj
DESTDIR     = ../../../lib/

INCLUDEPATH += ../defs  /usr/local/boost/ ../../core ../core

QT += xml

CONFIG += debug
