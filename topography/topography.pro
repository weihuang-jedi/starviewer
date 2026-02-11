#$Id: topography.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

#List of header files

 HEADERS =	topography.h

#List of source files

 SOURCES =	topography.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../nclapp

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
#QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-lnclapp
#LIBS		+=	-liconv.2.4.0

