QT += widgets
CONFIG += c++17 console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += src/main.cpp \
           src/bmp.cpp \
           src/lsb.cpp \
           src/gui_main.cpp
HEADERS += src/bmp.h \
           src/lsb.h
