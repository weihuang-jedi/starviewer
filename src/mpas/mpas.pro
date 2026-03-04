#$Id: mpas.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 
#QMAKE_CXXFLAGS += -DHASZGRID
#QMAKE_CXXFLAGS += -DMINIMIZE_MEMORY
 QMAKE_CXXFLAGS +=
 QMAKE_LFLAGS   +=

#List of header files

 HEADERS =	mpasgeometry.h		\
		mpastranslator.h	\
		mpascontroller.h	\
		mpas2dviewer.h

#		marchTetrahedron.h	\
#		mpas3dviewer.h		\

#List of source files

 SOURCES =	mpasgeometry.cpp	\
		mpastranslator.cpp	\
		mpascontroller.cpp	\
		mpas2dviewer.cpp

#		marchTetrahedron.cpp	\
#		mpas3dviewer.cpp	\

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../nclapp

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
#QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-ltranslator
 LIBS		+=	-lgeometry
 LIBS		+=	-lcoastline
 LIBS		+=	-lnclapp
 LIBS		+=	-liconv.2.4.0

