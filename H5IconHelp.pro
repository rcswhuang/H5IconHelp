#-------------------------------------------------
#
# Project created by QtCreator 2017-09-19T09:21:00
#
#-------------------------------------------------

QT += xml
QT += gui
QT += widgets
TARGET = H5IconHelp
TEMPLATE = lib
win32{
    DESTDIR = ../lib
}

unix{
DESTDIR = ../lib
}
DEFINES += H5ICONHELP_DLL

INCLUDEPATH += \
    ../include \
    ../include/H5IconGui \
    ../include/H5IconHelp
	
CONFIG += qt thread all
SOURCES += \
        hfonthelper.cpp \
        hiconhelper.cpp \
        hstation.cpp \
        hanaloguepage.cpp \
        hdigitalpage.cpp \
        hrelaypage.cpp \
        hiconproperty.cpp \
        hgraphpage.cpp \
        hgraph.cpp \
    hgraphhelper.cpp

HEADERS +=\
        ../include/hiconapi.h \
        ../include/H5IconHelp/h5iconhelpexport.h \
        ../include/H5IconHelp/hfonthelper.h \
        ../include/H5IconHelp/hiconhelper.h \
        ../include/H5IconHelp/hstation.h \
        ../include/H5IconHelp/hanaloguepage.h \
        ../include/H5IconHelp/hdigitalpage.h \
        ../include/H5IconHelp/hrelaypage.h \
        ../include/H5IconHelp/hiconproperty.h \
        ../include/H5IconHelp/hgraphpage.h \
        ../include/H5IconHelp/hgraph.h \
        ../include/H5IconHelp/hgraphhelper.h
		  
FORMS += \
    UI/iconproperty.ui \
    UI/analoguepage.ui \
    UI/digitalpage.ui \
    UI/relaypage.ui \
    UI/graphpage.ui

unix{

    LIBS += -L ../lib -lH5IconGui -ldatafile
}

win32{
   LIBS += \
        ../lib/H5IconGui.lib \
        ../lib/datafile.lib
}

win32{
    DEFINES += WIN32
}
unix {
    target.path = /users/huangw/lib
    INSTALLS += target
}

RESOURCES += \
    images.qrc
