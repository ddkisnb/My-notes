QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(./netapi/netapi.pri)
INCLUDEPATH += ./netapi

include(./audioapi/audioapi.pri)
INCLUDEPATH += ./audioapi


include(./videoapi/videoapi.pri)
INCLUDEPATH += ./videoapi

SOURCES += \
    chat.cpp \
    ckernel.cpp \
    logindialog.cpp \
    main.cpp \
    meetingdialog.cpp \
    roomdialog.cpp \
    screenread.cpp \
    user.cpp \
    usershowform.cpp

HEADERS += \
    chat.h \
    ckernel.h \
    logindialog.h \
    meetingdialog.h \
    roomdialog.h \
    screenread.h \
    user.h \
    usershowform.h

FORMS += \
    chat.ui \
    logindialog.ui \
    meetingdialog.ui \
    roomdialog.ui \
    user.ui \
    usershowform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    res.qrc
