QT       += core gui
INCLUDEPATH += E:\QScintilla-2.11.4\Qt4Qt5
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    finddialog.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    finddialog.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../QScintilla-2.11.4/build-qscintilla-Desktop_Qt_5_14_0_MinGW_64_bit-Debug/ -lqscintilla2_qt5
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../QScintilla-2.11.4/build-qscintilla-Desktop_Qt_5_14_0_MinGW_64_bit-Debug/ -lqscintilla2_qt5d
else:unix: LIBS += -L$$PWD/../../QScintilla-2.11.4/build-qscintilla-Desktop_Qt_5_14_0_MinGW_64_bit-Debug/ -lqscintilla2_qt5

INCLUDEPATH += $$PWD/../../QScintilla-2.11.4/build-qscintilla-Desktop_Qt_5_14_0_MinGW_64_bit-Debug
DEPENDPATH += $$PWD/../../QScintilla-2.11.4/build-qscintilla-Desktop_Qt_5_14_0_MinGW_64_bit-Debug

DISTFILES += \
    api_cpp.txt \
    images/cut.png \
    images/new.png \
    images/open.png \
    images/paste.png \
    images/save.png \
    images/search.png \
    images/setting.png

RESOURCES += \
    BabyCodeEditor.qrc
