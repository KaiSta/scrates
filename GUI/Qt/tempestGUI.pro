TEMPLATE = app

QT += qml quick widgets

# Enables C++11. Qt 5+ required
CONFIG += c++11

SOURCES += main.cpp \
    Models/container.cpp \
    Models/passwordstrengthchecker.cpp \
    Models/randomseedgenerator.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Models/container.h \
    Models/passwordstrengthchecker.h \
    Models/randomseedgenerator.h