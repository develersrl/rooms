CONFIG -= QT
QT -= core gui

TEMPLATE = lib
TARGET = rooms
INCLUDEPATH += . src lib/tinyxml lib/trex
OBJECTS_DIR = ./build
MOC_DIR = ./build
CONFIG += staticlib

!isEmpty(PYTHON_VERSION){
    DEFINES += WITH_PYTHON
    LIBS += -lpython$${PYTHON_VERSION}
    unix {
        INCLUDEPATH += /usr/include/python$${PYTHON_VERSION}
    } win32 {
        SUFFIX = $$replace(PYTHON_VERSION,\.,)
        INCLUDEPATH += C:/Python$${SUFFIX}/include
    }
    HEADERS += pythonvm.h
    SOURCES += src/pythonvm.cpp \
               src/pythonapi.cpp
}

# Workaround for a compilation error in OS X 10.8
# See also: http://qt-project.org/forums/viewthread/19106
macx-g++ {
    QMAKE_CXXFLAGS += -fpermissive
}

# Input
HEADERS += src/action.h \
           src/area.h \
           src/engine.h \
           src/event.h \
           src/eventsmanager.h \
           src/item.h \
           src/log.h \
           src/room.h \
           src/roomsmanager.h \
           src/dialog.h \
           src/roomsreader.h \
           src/gui.h \
           lib/tinyxml/tinystr.h \
           lib/tinyxml/tinyxml.h \
           src/versioning.h \
           src/animation.h \
           src/animationsmanager.h \
           src/timer.h \
           src/csparser.h \
           lib/trex/TRexpp.h \
    src/csmanager.h \
    src/delayedanimation.h \
    src/cutscenes_defaults.h
SOURCES += src/action.cpp \
           src/area.cpp \
           src/engine.cpp \
           src/event.cpp \
           src/eventsmanager.cpp \
           src/item.cpp \
           src/log.cpp \
           src/room.cpp \
           src/roomsmanager.cpp \
           src/dialog.cpp \
           src/roomsreader.cpp \
           src/gui.cpp \
           lib/tinyxml/tinystr.cpp \
           lib/tinyxml/tinyxml.cpp \
           lib/tinyxml/tinyxmlerror.cpp \
           lib/tinyxml/tinyxmlparser.cpp \
           src/versioning.cpp \
           src/animation.cpp \
           src/animationsmanager.cpp \
           src/timer.cpp \
           src/csparser.cpp \
           lib/trex/trex.c \
    src/csmanager.cpp \
    src/delayedanimation.cpp
