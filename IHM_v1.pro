QT       += core gui
QT += core
QT += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GlobalFunc.cpp \
    doubleSprofile.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

HEADERS += \
    GlobalFunc.h \
    doubleSprofile.h \
    mainwindow.h \
    qcustomplot.h \
    serialThread.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    resources/aboveArrow.jpg \
    resources/belowArrow.jpg \
    resources/images.png \
    resources/leftArrow.jpg \
    resources/rightArrow.jpg

RESOURCES += \
    resources.qrc
