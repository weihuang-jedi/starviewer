#$Id: pop.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

#List of header files

 HEADERS =	popgeometry.h		\
		poptranslator.h		\
		popcontroller.h		\
		pop2dviewer.h

#List of source files

 SOURCES =	popgeometry.cpp		\
		poptranslator.cpp	\
		popcontroller.cpp	\
		pop2dviewer.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../nclapp

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
#QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-lgeometry
 LIBS		+=	-lcoastline
 LIBS		+=	-ltranslator
 LIBS		+=	-lnclapp
 LIBS		+=	-liconv.2.4.0

