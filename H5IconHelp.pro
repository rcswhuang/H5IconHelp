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
        ../include/H5IconHelp/hgraphpage.h  \
        ../include/H5IconHelp/hgraphhelper.h
		  
FORMS += \
    UI/iconproperty.ui \
    UI/analoguepage.ui \
    UI/digitalpage.ui \
    UI/relaypage.ui \
    UI/graphpage.ui

RESOURCES += \
    images.qrc

LIBS += -L../lib -lH5IconGui -ldatafile
unix{
    UI_DIR = temp/ui
    MOC_DIR = temp/moc
    OBJECTS_DIR	= temp/obj
    LIBS += \

}

win32{
    UI_DIR = temp/ui
    MOC_DIR = temp/moc
    OBJECTS_DIR	= temp/obj
    LIBS += \
}

