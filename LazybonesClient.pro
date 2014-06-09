TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp \
    lazybonesclient.cpp \
    lazybonesprotocol.cpp \
    lazybonesconnection.cpp \
    channeluserinfo.cpp \
    screenimageprovider.cpp \
    imagetiledata.cpp \
    screenview.cpp \
    screensetting.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    lazybonesclient.h \
    lazybonesprotocol.h \
    lazybonesconnection.h \
    channeluserinfo.h \
    screenimageprovider.h \
    imagetiledata.h \
    screenview.h \
    screensetting.h
