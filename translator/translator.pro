#$ID

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	basetranslator.h	\
		generaltranslator.h

#List of source files

 SOURCES =	basetranslator.cpp	\
		generaltranslator.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../marchingCube
 INCLUDEPATH	+=	../nclapp
 INCLUDEPATH	+=	../view
 INCLUDEPATH	+=	../control
 INCLUDEPATH	+=	../contour

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
#QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lcoastline
 LIBS		+=	-lgeometry
 LIBS		+=	-lshared
 LIBS		+=	-lmarchingCube
 LIBS		+=	-lnclapp
 LIBS		+=	-lview
 LIBS		+=	-lcontour
 LIBS		+=	-lcontrol
 LIBS		+=	-liconv.2.4.0

