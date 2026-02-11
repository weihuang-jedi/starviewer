#$ID

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	gl_viewer.h	\
		ncl_viewer.h	\
		pixel_viewer.h	\
		spreadsheet.h

#List of source files

 SOURCES =	gl_viewer.cpp	\
		ncl_viewer.cpp	\
		pixel_viewer.cpp\
		spreadsheet.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../marchingCube
 INCLUDEPATH	+=	../nclapp
 INCLUDEPATH	+=	../contour

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
#QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lcoastline
 LIBS		+=	-lgeometry
 LIBS		+=	-lshared
 LIBS		+=	-lmarchingCube
 LIBS		+=	-lnclapp
 LIBS		+=	-lcontour
 LIBS		+=	-liconv.2.4.0

