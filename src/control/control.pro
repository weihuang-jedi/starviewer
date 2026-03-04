#$Id: control.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	controller.h

#List of source files

 SOURCES =	controller.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../nclapp
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../marchingCube
 INCLUDEPATH	+=	../nclapp
 INCLUDEPATH	+=	../view
 INCLUDEPATH	+=	../contour

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)

 LIBS		+=	-lcoastline
 LIBS		+=	-lshared
 LIBS		+=	-lnclapp
 LIBS		+=	-lgeometry
 LIBS		+=	-lmarchingCube
 LIBS		+=	-lnclapp
 LIBS		+=	-lview
 LIBS		+=	-lcontour

