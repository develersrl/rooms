######################################################################
# Automatically generated by qmake (2.01a) mar dic 21 11:04:44 2010
######################################################################

TEMPLATE = app
TARGET = test.exe
INCLUDEPATH += . ../../frontend ../../src ../../lib/tinyxml ../../lib/cppunit-build/include ../../lib/trex
LIBS += ../../lib/cppunit-build/lib/libcppunit.a
OBJECTS_DIR = ./build
MOC_DIR = ./build

# Input
HEADERS += ../../src/action.h \
           ../../src/area.h \
           ../../src/engine.h \
           ../../src/event.h \
           ../../src/dialog.h \
           ../../src/eventsmanager.h \
           ../../src/item.h \
           ../../src/log.h \
           ../../src/room.h \
           ../../src/roomsmanager.h \
           ../../src/roomsreader.h \
           ../../src/gui.h \
           ../../src/versioning.h \
           ../../src/animation.h \
           ../../src/animationsmanager.h \
           ../../src/timer.h \
           ../../lib/tinyxml/tinystr.h \
           ../../lib/tinyxml/tinyxml.h \
           ../../src/csparser.h \
           ../../lib/trex/TRexpp.h \
           ../../src/csmanager.h \
           ../../src/delayedanimation.h

SOURCES += main.cpp \
           enginetests.cpp \
           ../../src/action.cpp \
           ../../src/dialog.cpp \
           ../../src/area.cpp \
           ../../src/engine.cpp \
           ../../src/event.cpp \
           ../../src/eventsmanager.cpp \
           ../../src/item.cpp \
           ../../src/log.cpp \
           ../../src/room.cpp \
           ../../src/roomsmanager.cpp \
           ../../src/roomsreader.cpp \
           ../../src/gui.cpp \
           ../../src/versioning.cpp \
           ../../src/animation.cpp \
           ../../src/animationsmanager.cpp \
           ../../src/timer.cpp \
           ../../lib/tinyxml/tinystr.cpp \
           ../../lib/tinyxml/tinyxml.cpp \
           ../../lib/tinyxml/tinyxmlerror.cpp \
           ../../lib/tinyxml/tinyxmlparser.cpp \
           ../../src/csparser.cpp \
           ../../lib/trex/trex.c \
           ../../src/csmanager.cpp \
           ../../src/delayedanimation.cpp
