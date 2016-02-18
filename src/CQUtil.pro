TEMPLATE = lib

TARGET = CQUtil

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

# Input
HEADERS += \
../include/CQUtil.h \
../include/CQApp.h \
../include/CQMainWindow.h \
../include/CQWindow.h \
../include/CQImage.h \
../include/CQFont.h \
../include/CQMenu.h \
../include/CQWidgetUtil.h \
../include/CQColorChooser.h \
../include/CQFontChooser.h \
../include/CQAlphaButton.h \
../include/CQPixmapCache.h \
../include/CQStyle.h \

SOURCES += \
CQUtil.cpp \
CQApp.cpp \
CQMainWindow.cpp \
CQWindow.cpp \
CQImage.cpp \
CQFont.cpp \
CQColorChooser.cpp \
CQFontChooser.cpp \
CQAlphaButton.cpp \
CQPixmapCache.cpp \
CQStyle.cpp \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
. \
../include \
../../CFont/include \
../../CImageLib/include \
../../CFile/include \
../../CUtil/include \
../../CMath/include \
../../CStrUtil/include \
../../CConfig/include \
../../CRegExp/include \
../../COS/include \
