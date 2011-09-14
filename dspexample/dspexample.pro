# -------------------------------------------------
# Project created by QtCreator 2010-12-06T19:18:23
# -------------------------------------------------
QT += core \
    gui
TARGET = dspexample
TEMPLATE = app
LIBS += -lfftw3f \
    -ljack \
    -lsndfile
SOURCES += fileManager.cpp \
    fir.cpp \
    main.cpp \
    mainwindow.cpp \
    equalizer.cpp \
    freqFilter.cpp \
    jack.cpp \
    dspsystem.cpp \
    combfilter.cpp \
    reverb.cpp
HEADERS += fileManager.h \
    fir.h \
    mainwindow.h \
    equalizer.h \
    freqFilter.h \
    jack.h \
    processor.h \
    dspsystem.h \
    combfilter.h \
    reverb.h
FORMS += mainwindow.ui
