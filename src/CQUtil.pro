TEMPLATE = lib

TARGET = CQUtil

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQUTIL_PALETTE \
-DCQUTIL_PEN \
-DCQUTIL_BRUSH \
-DCQUTIL_FONT \
-DCQUTIL_IMAGE \
-DCQUTIL_GRADIENT \
-DCBRUSH_IMAGE \
-DCQUTIL_LINE_DASH \
-DQT_NO_X11 \

CONFIG += c++17

MOC_DIR = .moc

SOURCES += \
CQUtil.cpp \
CQUtilRGBA.cpp \
CQUtilEvent.cpp \
CQMsgHandler.cpp \
\
CQApp.cpp \
CQAppOptions.cpp \
CQDialog.cpp \
\
CQMainWindow.cpp \
CQWorkspace.cpp \
CQToolBar.cpp \
CQWindow.cpp \
\
CQPenUtil.cpp \
CQBrushUtil.cpp \
\
CQImage.cpp \
CQImageUtil.cpp \
\
CQPixmapCache.cpp \
CQStyle.cpp \
CQStyleMgr.cpp \
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
CQComboSlider.cpp \
CQSlider.cpp \
CQLabel.cpp \
\
CQImageButton.cpp \
CQPixmapButton.cpp \
CQIconButton.cpp \
CQIconRadio.cpp \
CQIconLabel.cpp \
CQRadioButtons.cpp \
\
CQPoint2DEdit.cpp \
CQBBox2DEdit.cpp \
CQRectEdit.cpp \
CQFloatEdit.cpp \
CQAutoHide.cpp \
CQAngleSpinBox.cpp \
CQToolTip.cpp \
CQCheckBox.cpp \
\
CQTabWidget.cpp \
CQTabWidgetTabBar.cpp \
\
CQTabSplit.cpp \
CQTabSplitSplitter.cpp \
CQTabSplitSplitterHandle.cpp \
CQTabSplitSplitterTool.cpp \
\
CQScrollArea.cpp \
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
\
CQDragLabel.cpp \
\
CQWidgetFactory.cpp \
\
CQStrUtil.cpp \
\
CQEnumCombo.cpp \
\
CQTrie.cpp \
\
CQSwitchLineEdit.cpp \
\
CQFlowLayout.cpp \
\
CQFlagsCombo.cpp \
\
CQHtmlTextPainter.cpp \
\
CQMatrixStack2D.cpp \
\
CQLabelImage.cpp \

HEADERS += \
../include/CQUtil.h \
../include/CQMsgHandler.h \
\
../include/CQApp.h \
../include/CQAppOptions.h \
../include/CQDialog.h \
\
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
../include/CQStyleMgr.h \
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
../include/CQComboSlider.h \
../include/CQSlider.h \
../include/CQLabel.h \
\
../include/CQImageButton.h \
../include/CQPixmapButton.h \
../include/CQIconButton.h \
../include/CQIconRadio.h \
../include/CQIconLabel.h \
../include/CQRadioButtons.h \
\
../include/CQPoint2DEdit.h \
../include/CQBBox2DEdit.h \
../include/CQRectEdit.h \
../include/CQFloatEdit.h \
../include/CQAutoHide.h \
../include/CQAngleSpinBox.h \
../include/CQToolTip.h \
../include/CQCheckBox.h \
\
../include/CQTabWidget.h \
../include/CQTabWidgetTabBar.h \
\
../include/CQTabSplit.h \
../include/CQTabSplitSplitter.h \
../include/CQTabSplitSplitterHandle.h \
../include/CQTabSplitSplitterTool.h \
\
../include/CQScrollArea.h \
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
\
../include/CQDragLabel.h \
\
../include/CQWidgetFactory.h \
\
../include/CQStrUtil.h \
\
../include/CQEnumCombo.h \
\
../include/CQTrie.h \
\
../include/CQSwitchLineEdit.h \
\
../include/CQFlowLayout.h \
\
../include/CQFlagsCombo.h \
\
../include/CQHtmlTextPainter.h \
\
../include/CQMatrixStack2D.h \
\
../include/CQLabelImage.h \

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
