QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutwindow.cpp \
    contactwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    project.cpp \
    projectmodel.cpp \
    projectmodelproxy.cpp \
    settingswindow.cpp \
    taskitem.cpp \
    texteditwithfocusoutevent.cpp \
    usersettings.cpp

HEADERS += \
    aboutwindow.h \
    contactwindow.h \
    mainwindow.h \
    project.h \
    projectmodel.h \
    projectmodelproxy.h \
    settingswindow.h \
    taskitem.h \
    texteditwithfocusoutevent.h \
    usersettings.h

FORMS += \
    aboutwindow.ui \
    contactwindow.ui \
    mainwindow.ui \
    settingswindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RC_ICONS = ScatterbrainLogo.ico
