TEMPLATE = app

QT += qml quick widgets

# Enables C++11. Qt 5+ required
CONFIG += c++11

SOURCES += main.cpp \
    Models/container.cpp \
    Models/passwordstrengthchecker.cpp \
    Models/randomseedgenerator.cpp \
    include/pugixml.cpp \ # only if needed for the GUI
    Models/settings.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Models/container.h \
    Models/passwordstrengthchecker.h \
    Models/randomseedgenerator.h \
    include/pugixml.hpp \
    include/pugiconfig.hpp \
    Models/settings.h

# Poco C++
INCLUDEPATH += /usr/local/include
unix:LIBS += -L"/usr/local/lib" -lPocoFoundationd -lPocoUtild -lPocoXMLd
unix:LIBS += -L"/usr/local/lib" -lPocoFoundation  -lPocoUtil -lPocoXML

# Crypto++
INCLUDEPATH += /usr/include
unix: LIBS += /usr/lib/libcryptopp.a

# tempest
INCLUDEPATH += $$PWD/../../delivery/include
unix: LIBS += $$PWD/../../delivery/libcrypt.a

#TOOD: TARGET, e.g. for OS X 10.10, 10.9, 10.8...
