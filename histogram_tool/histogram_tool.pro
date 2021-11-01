QT += testlib concurrent

TARGET = histogramTool
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
