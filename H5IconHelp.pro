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
    DESTDIR = $$(WF_ROOT)/lib
}

unix{
DESTDIR = /users/huangw/lib
}
DEFINES += H5ICONHELP_DLL

INCLUDEPATH += \
    ../include \
    ../include/H5IconGui \
    ../include/H5IconHelp
	
CONFIG += qt thread all
SOURCES += \
        hpropertydlg.cpp \
        hfonthelper.cpp \
        hiconhelper.cpp \
        hdatapropertydlg.cpp \
    hstation.cpp

HEADERS +=\
        ../include/hiconapi.h \
        ../include/H5IconHelp/h5iconhelpexport.h \
        ../include/H5IconHelp/hpropertydlg.h \
        ../include/H5IconHelp/hfonthelper.h \
        ../include/H5IconHelp/hiconhelper.h \
        ../include/H5IconHelp/hdatapropertydlg.h \
    ../include/H5IconHelp/hstation.h
		  
FORMS += \
    UI/propertydlg.ui \
    UI/datapropertydlg.ui

unix{

    LIBS += -L ../lib/ -lH5IconGui
}

win32{
   LIBS += \
        ../lib/H5IconGui.lib
}

win32{
    DEFINES += WIN32
}
unix {
    target.path = /users/huangw/lib
    INSTALLS += target
}
