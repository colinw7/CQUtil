TEMPLATE = lib

TARGET = CQUtil

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

SOURCES += \
CQUtil.cpp \
CQApp.cpp \
CQMainWindow.cpp \
CQToolBar.cpp \
CQWindow.cpp \
CQImage.cpp \
CQFont.cpp \
\
CQPixmapCache.cpp \
CQStyle.cpp \
\
CQColorChooser.cpp \
CQFontChooser.cpp \
CQAlphaButton.cpp \
CQIntegerEdit.cpp \
CQRealEdit.cpp \
CQRealSpin.cpp \
CQPoint2DEdit.cpp \
CQBBox2DEdit.cpp \
CQImageButton.cpp \
CQFloatEdit.cpp \
CQAutoHide.cpp \
CQRadioButtons.cpp \
CQAngleSpinBox.cpp \
CQToolTip.cpp \
\
CQWinWidget.cpp \

HEADERS += \
../include/CQUtil.h \
../include/CQApp.h \
../include/CQMainWindow.h \
../include/CQToolBar.h \
../include/CQWindow.h \
../include/CQImage.h \
../include/CQFont.h \
\
../include/CQPixmapCache.h \
../include/CQStyle.h \
../include/CQMenu.h \
../include/CQWidgetUtil.h \
\
../include/CQColorChooser.h \
../include/CQFontChooser.h \
../include/CQAlphaButton.h \
../include/CQIntegerEdit.h \
../include/CQRealEdit.h \
../include/CQRealSpin.h \
../include/CQPoint2DEdit.h \
../include/CQBBox2DEdit.h \
../include/CQImageButton.h \
../include/CQFloatEdit.h \
../include/CQAutoHide.h \
../include/CQRadioButtons.h \
../include/CQAngleSpinBox.h \
../include/CQToolTip.h \
\
../include/CQWinWidget.h \

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
