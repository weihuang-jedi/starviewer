#$Id: radar.pro 14613 2013-11-04 20:48:49Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

 QMAKE_CXXFLAGS += -D_RSL4NV

#List of header files

 HEADERS =	radarreader.h		\
		radartranslator.h	\
		radarcontroller.h	\
		radar2dviewer.h		\
		radar3dviewer.h

#List of source files

 SOURCES =	radarreader.cpp		\
		radartranslator.cpp	\
		radarcontroller.cpp	\
		radar2dviewer.cpp	\
		radar3dviewer.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../coastline2
 INCLUDEPATH	+=	../marchTetrahedron

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(NV_HOME)/lib
 QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-ltranslator
 LIBS		+=	-lcoastline2
 LIBS		+=	-lmarchTetrahedron
 LIBS		+=	-liconv.2.4.0

