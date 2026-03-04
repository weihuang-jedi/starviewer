#$Id: lic.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

#List of header files

 HEADERS =	point.h			\
		noise.h			\
		reggrid.h		\
		streamline.h		\
		licvector.h		\
		lic.h

#List of source files

 SOURCES =	point.cpp		\
		noise.cpp		\
		reggrid.cpp		\
		streamline.cpp		\
		licvector.cpp		\
		lic.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../control
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../nclapp

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
 QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-lcontrol
 LIBS		+=	-ltranslator
 LIBS		+=	-lcoastline
 LIBS		+=	-lnclapp
 LIBS		+=	-liconv.2.4.0

