#-------------------------------------------------
#
# Project created by QtCreator 2016-11-14T17:50:50
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = object_marker
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app



SOURCES += \
    src/classify_images.cpp \



INCLUDEPATH +=
                /home/edgar/opencv/include \

LIBS += -L/home/edgar/opencv/build/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc

