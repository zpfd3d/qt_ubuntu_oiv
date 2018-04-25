#-------------------------------------------------
#
# Project created by QtCreator 2018-03-27T10:11:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VIO
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
QT += serialport
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


win32 {
        #LIBS += -L$$PWD/Depends/win32/vs2013shared			-lMVSDKmd
        #LIBS += -L$$PWD/Depends/win32/vs2013shared			-lVideoRender
        #LIBS += -L$$PWD/Depends/win32/vs2013shared			-lImageConvert
}
else {
        QMAKE_LIBS_OPENGL =
        DEFINES += QT_NO_DEBUG_OUTPUT LINUX64 QT_NO_OPENGL
        QMAKE_CXXFLAGS_RELEASE += -mssse3
        LIBS += -L/opt/DahuaTech/MVViewer/lib/	-lMVSDK
        LIBS += -L/opt/DahuaTech/MVViewer/lib/	-llog4cpp
        LIBS += -L/opt/DahuaTech/MVViewer/lib/	-lImageConvert
        LIBS += -L/opt/DahuaTech/MVViewer/lib/	-lVideoRender
        LIBS += -L/opt/DahuaTech/MVViewer/lib/GenICam/bin/Linux64_x64 -lGCBase_gcc421_v3_0 -lGenApi_gcc421_v3_0 -lLog_gcc421_v3_0 -llog4cpp_gcc421_v3_0 -lNodeMapData_gcc421_v3_0 -lXmlParser_gcc421_v3_0 -lMathParser_gcc421_v3_0 -lrt
        #LIBS += -L/home/idp/Workspace/VIO/libs -lVISLAM
        LIBS += -L/lib -lVISLAM
}


INCLUDEPATH = /opt/DahuaTech/MVViewer/include
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    system_info.cpp \
    ini_reader.cpp \
    IMUReader.cpp \
    aboutdialog.cpp \
    cameramanager.cpp \
    camerawnd.cpp \
    globaldata.cpp

HEADERS += \
        mainwindow.h \
    system_info.h \
    ini_reader.h \
    IMUReader.h \
    aboutdialog.h \
    cameramanager.h \
    messageque.h \
    camerawnd.h \
    globaldata.h \
    vislam_core.h

FORMS += \
        mainwindow.ui \
    aboutdialog.ui \
    camerawnd.ui

RESOURCES += \
    vio.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opt/DahuaTech/MVViewer/lib/release/ -lImageConvert
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opt/DahuaTech/MVViewer/lib/debug/ -lImageConvert
else:unix: LIBS += -L$$PWD/../../../../opt/DahuaTech/MVViewer/lib/ -lImageConvert
LIBS += -L/opt/DahuaTech/MVViewer/lib/GenICam/bin/Linux64_x64 -lGCBase_gcc421_v3_0 -lGenApi_gcc421_v3_0 -lLog_gcc421_v3_0 -llog4cpp_gcc421_v3_0 -lNodeMapData_gcc421_v3_0 -lXmlParser_gcc421_v3_0 -lMathParser_gcc421_v3_0 -lrt
        LIBS += -L$$PWD/../../../../opt/DahuaTech/MVViewer/lib/	-lMVSDK
        LIBS += -L$$PWD/../../../../opt/DahuaTech/MVViewer/lib/	-llog4cpp
        LIBS += -L$$PWD/../../../../opt/DahuaTech/MVViewer/lib/	-lImageConvert
        LIBS += -L$$PWD/../../../../opt/DahuaTech/MVViewer/lib/	-lVideoRender
INCLUDEPATH += $$PWD/../../../../opt/DahuaTech/MVViewer/include
DEPENDPATH += $$PWD/../../../../opt/DahuaTech/MVViewer/include
#CONFIG += console
