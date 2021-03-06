QT += core widgets serialport printsupport
CONFIG += c++11

#only for static release
QMAKE_LFLAGS_RELEASE += -static -static-libgcc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    protocolmanager.cpp \
    dev/serialhandler.cpp \
    coreserver.cpp \
    protocoldata.cpp \
    gui/mainwindow.cpp \
    settingswizard.cpp \
    recordModel/chartrecordmodel.cpp \
    recordModel/customrecorddelegate.cpp \
    console.cpp \
    gui/createparamdialog.cpp \
    qcustomplot/qcustomplot.cpp \
    shell.cpp \
    gui/eloplotter.cpp \
    logger.cpp

HEADERS += \
    protocolmanager.h \
    coreserver.h \
    dev/serialhandler.h \
    protocoldata.h \
    gui/mainwindow.h \
    settingswizard.h \
    recordModel/chartrecordmodel.h \
    recordModel/customrecorddelegate.h \
    console.h \
    gui/createparamdialog.h \
    qcustomplot/qcustomplot.h \
    shell.h \
    gui/eloplotter.h \
    logger.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    gui/mainwindow.ui \
    gui/createparamdialog.ui

RESOURCES += \
    res.qrc
