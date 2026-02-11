#Includes common configuration for all subdirectory .pro files.

 DESTDIR = /usr/local/lib/nv
#DESTDIR = /Users/weihuang/src/nv/lib
 message(The project will be installed in $$DESTDIR)

 TEMPLATE = lib app

#CONFIG	  += staticlib
 CONFIG	  += ordered

 WARNINGS += -Wall

#QMAKE_LINK = g++
#QMAKE_CXX = g++
 QMAKE_LINK = clang++
 QMAKE_CXX = clang++
 QMAKE_CXXFLAGS = -g -DUSENCL -DGL_GLEXT_PROTOTYPES
#QMAKE_CXXFLAGS = -omp -g
#QMAKE_CC = gcc
 QMAKE_CC = clang
 QMAKE_CFLAGS = -g -DUSENCL
#QMAKE_CFLAGS = -g -DUSENCL -DQT_NO_STL
#QMAKE_CFLAGS = -omp -g
 QMAKE_LFLAGS = -g
 QMAKE_LDFLAGS  = -L/usr/local/lib
 QMAKE_CPPFLAGS = -I/usr/local/include

#List of needed incs:

#INCLUDEPATH += .
 INCLUDEPATH += /opt/local/include
#INCLUDEPATH += /usr/include
 INCLUDEPATH += /usr/local/include
#INCLUDEPATH += /opt/local/include
 INCLUDEPATH += /opt/X11/include
#INCLUDEPATH += /usr/X11/include
 INCLUDEPATH += /usr/X11/include/freetype2
 INCLUDEPATH +=	/Users/weihuang/lib/NCLDEV/include
#INCLUDEPATH +=	/Users/weihuang/lib/radx/20140113/include

 QMAKE_RPATHDIR += /usr/lib
#QMAKE_RPATHDIR += /opt/local/lib

#LIBS		+= -F/Library/Frameworks
 LIBS		+= -liconv.2.4.0

#Include OpenGL support
 QT += opengl

