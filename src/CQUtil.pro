TEMPLATE = lib

TARGET = CQUtil

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++14 -DCQUTIL_IMAGE -DCQUTIL_FONT

MOC_DIR = .moc

SOURCES += \
CQUtil.cpp \
CQMsgHandler.cpp \
\
CQApp.cpp \
CQMainWindow.cpp \
CQWorkspace.cpp \
CQToolBar.cpp \
CQWindow.cpp \
\
CQImage.cpp \
CQImageUtil.cpp \
\
CQPixmapCache.cpp \
CQStyle.cpp \
\
CQColorChooser.cpp \
CQAlphaButton.cpp \
CQColorEdit.cpp \
\
CQFont.cpp \
CQFontChooser.cpp \
CQFontDialog.cpp \
CQFontEdit.cpp \
CQFontListView.cpp \
CQFontUtil.cpp \
\
CQIntegerEdit.cpp \
CQRealEdit.cpp \
CQIntegerSpin.cpp \
CQRealSpin.cpp \
\
CQPoint2DEdit.cpp \
CQBBox2DEdit.cpp \
CQRectEdit.cpp \
CQImageButton.cpp \
CQFloatEdit.cpp \
CQAutoHide.cpp \
CQRadioButtons.cpp \
CQAngleSpinBox.cpp \
CQToolTip.cpp \
CQTabWidget.cpp \
CQCheckBox.cpp \
CQTabSplit.cpp \
\
CQTreeView.cpp \
CQTableView.cpp \
CQTableWidget.cpp \
CQHeaderView.cpp \
\
CQAlignEdit.cpp \
CQIconCombo.cpp \
CQWidgetMenu.cpp \
CQSwitch.cpp \
CQGroupBox.cpp \
CQLineEdit.cpp \
\
CQLineDash.cpp \
\
CQWinWidget.cpp \
\
CQStrParse.cpp \

HEADERS += \
../include/CQUtil.h \
../include/CQMsgHandler.h \
\
../include/CQApp.h \
../include/CQMainWindow.h \
../include/CQToolBar.h \
../include/CQWorkspace.h \
../include/CQMdiArea.h \
../include/CQWindow.h \
\
../include/CQImage.h \
../include/CQImageUtil.h \
\
../include/CQPixmapCache.h \
../include/CQStyle.h \
\
../include/CQMenu.h \
../include/CQWidgetUtil.h \
\
../include/CQColorChooser.h \
../include/CQAlphaButton.h \
../include/CQColorEdit.h \
\
../include/CQFont.h \
../include/CQFontChooser.h \
../include/CQFontDialog.h \
../include/CQFontEdit.h \
../include/CQFontListView.h \
../include/CQFontUtil.h \
\
../include/CQIntegerEdit.h \
../include/CQRealEdit.h \
\
../include/CQIntegerSpin.h \
../include/CQRealSpin.h \
\
../include/CQPoint2DEdit.h \
../include/CQBBox2DEdit.h \
../include/CQRectEdit.h \
../include/CQImageButton.h \
../include/CQFloatEdit.h \
../include/CQAutoHide.h \
../include/CQRadioButtons.h \
../include/CQAngleSpinBox.h \
../include/CQToolTip.h \
../include/CQTabWidget.h \
../include/CQCheckBox.h \
../include/CQTabSplit.h \
\
../include/CQTreeView.h \
../include/CQTableView.h \
../include/CQTableWidget.h \
../include/CQHeaderView.h \
\
../include/CQAlignEdit.h \
../include/CQIconCombo.h \
../include/CQWidgetMenu.h \
../include/CQSwitch.h \
../include/CQGroupBox.h \
../include/CQLineEdit.h \
\
../include/CQLineDash.h \
\
../include/CQWinWidget.h \
\
../include/CQStrParse.h \

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
