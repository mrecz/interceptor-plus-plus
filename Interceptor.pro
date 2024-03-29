QT       += core gui
QT       += svgwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += /std:c++latest

LIBS += -lUser32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customeventfilter.cpp \
    graphicsscene.cpp \
    hotkey.cpp \
    interceptor.cpp \
    main.cpp \
    mainwindow.cpp \
    overlay.cpp \
    sceneitem.cpp \
    updatebox.cpp

HEADERS += \
    customeventfilter.h \
    graphicsscene.h \
    hotkey.h \
    interceptor.h \
    mainwindow.h \
    overlay.h \
    sceneitem.h \
    updatebox.h \
    utilities.h

FORMS += \
    mainwindow.ui \
    updatebox.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_FILE = rcs.rc

RESOURCES += \
    resources.qrc
