# Project file for camse
# Wei Huang

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

#List of header files

 HEADERS =	camsetranslator.h	\
		camsecontroller.h	\
		camsegeometry.h		\
		camse2dviewer.h

#List of source files

 SOURCES =	camsetranslator.cpp	\
		camsecontroller.cpp	\
		camsegeometry.cpp	\
		camse2dviewer.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../nclapp

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
#QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-lcoastline
 LIBS		+=	-ltranslator
 LIBS		+=	-lnclapp
 LIBS		+=	-liconv.2.4.0

