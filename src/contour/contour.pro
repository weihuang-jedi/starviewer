#$Id: contour.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib
 CONFIG += -DBuildQtEnabled -DUSENCL

#List of header files

 HEADERS =	contour.h

#List of source files

 SOURCES =	contour.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../nclapp
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../shared

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
 QMAKE_LIBDIR	+=	/usr/lib
 QMAKE_LIBDIR	+=	/opt/local/lib

 LIBS		+=	-lnclapp
 LIBS		+=	-lcoastline
 LIBS		+=	-lgeometry
 LIBS		+=	-lshared
 LIBS		+=	-lcairo
 LIBS		+=	-liconv.2.4.0

