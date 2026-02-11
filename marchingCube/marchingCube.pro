#$Id: marchingCube.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	marchingcube.h	\
		leveler.h

#List of source files

 SOURCES =	marchingcube.cpp	\
		leveler.cpp

#List of extra needed incs:

#INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../shared

#List of extra needed libs:
 QMAKE_LIBDIR	+=	$(DESTDIR)

 LIBS		+=	-lgeometry
 LIBS		+=	-lshared

