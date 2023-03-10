QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    thread_ffmpeg_laliu.cpp

HEADERS += \
    mainwindow.h \
    thread_ffmpeg_laliu.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += D:\1_install\opencv_MINGW\OpenCV-MinGW-Build\include\

#LIBS += D:\1_install\opencv_MINGW\OpenCV-MinGW-Build\x64\mingw\lib\libopencv_*.a

INCLUDEPATH += $$PWD/ffmpeg/include

LIBS += $$PWD/ffmpeg/lib/avcodec.lib \
        $$PWD/ffmpeg/lib/avdevice.lib \
        $$PWD/ffmpeg/lib/avfilter.lib \
        $$PWD/ffmpeg/lib/avformat.lib \
        $$PWD/ffmpeg/lib/avutil.lib \
        $$PWD/ffmpeg/lib/postproc.lib \
        $$PWD/ffmpeg/lib/swresample.lib \
        $$PWD/ffmpeg/lib/swscale.lib

## Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -LD:/duanmingman/opencv/build/x64/vc14/lib/ -lopencv_world400
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/duanmingman/opencv/build/x64/vc14/lib/ -lopencv_world400d

INCLUDEPATH+= D:/duanmingman/opencv/build/include/opencv2 \
D:/duanmingman/opencv/build/include

DEPENDPATH += D:/duanmingman/opencv/build/include/opencv2 \
D:/duanmingman/opencv/build/include
