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
    ../include/H5IconGui
	
CONFIG += qt thread all
SOURCES += \
        hpropertydlg.cpp \
    hfonthelper.cpp \
    hiconhelper.cpp \
    hdatapropertydlg.cpp

HEADERS +=\
          hpropertydlg.h \
    hfonthelper.h \
    hiconhelper.h \
    hdatapropertydlg.h
		  
FORMS += \
    UI/propertydlg.ui \
    UI/hdatapropertydlg.ui

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
