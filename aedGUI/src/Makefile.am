SUBDIRS = tinyxml
AM_LDFLAGS =  @SDL_LIBS@
AM_CXXFLAGS = `@SDL_CONFIG@ --cflags` -I../include/

EXTRA_DIST = SDL_ttf.h SDL_ttf.c SDL_loadso.c tinyxml/tinyxml.h tinyxml/tinystr.h

lib_LTLIBRARIES = libaedGUI.la
libaedGUI_la_LDFLAGS = -no-undefined -version-info @AEDGUI_CURRENT@:@AEDGUI_REVISION@:@AEDGUI_AGE@ -release @AEDGUI_RELEASE@

TINYXML_SRCS = tinyxml/tinystr.cpp tinyxml/tinyxmlerror.cpp \
	tinyxml/tinyxmlparser.cpp tinyxml/tinyxml.cpp

libaedGUI_la_SOURCES = 	$(TINYXML_SRCS) \
			aedXmlFile.cpp \
			drawlibs.cpp \
			aedMenu.cpp \
			aedMenuBar.cpp \
			aedApp.cpp \
			aedTheme.cpp \
			aedObject.cpp \
			aedWidget.cpp \
			aedWindow.cpp \
			aedStaticText.cpp \
			aedStaticRTF.cpp \
			aedButton.cpp \
			aedTextBox.cpp \
			aedDialogBox.cpp \
			aedProgressBar.cpp \
			aedFont.cpp \
			aedFrame.cpp \
			aedCheckBox.cpp \
			aedUpdateManager.cpp \
			aedSignal.cpp \
			aedImage.cpp \
			aedImageBank.cpp \
			aedSlider.cpp \
			aedScrollBar.cpp \
			aedStaticImage.cpp \
			aedAnimatedImage.cpp \
			aedRadioButtonGroup.cpp \
			aedTimer.cpp \
			aedListBox.cpp \
			aedSpinner.cpp \
			aedCursor.cpp \
			aedLog.cpp \
			aedSizer.cpp \
			aedBoxSizer.cpp

