#-------------------------------------------------
#
# Project created by QtCreator 2015-07-24T16:23:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = Karaoke
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    helperkaraokelabel.cpp \
    karaokeword.cpp \
    lyricASS.cpp \
    lyricframe.cpp \
    lyricJson.cpp \
    lyricLRC.cpp \
    lyricPrepare.cpp \
    playlistwindow.cpp \
    qdirectslider.cpp \
    RubyChar.cpp \
    sentenceholder.cpp \
    Settings.cpp

HEADERS  += mainwindow.h \
    helperkaraokelabel.h \
    karaokeword.h \
    lyricframe.h \
    lyricJson.h \
    playlistwindow.h \
    qdirectslider.h \
    RubyChar.h \
    sentenceholder.h \
    Settings.h

FORMS    += mainwindow.ui \
    playlistwindow.ui
