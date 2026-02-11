#$Id: wrf.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

#List of header files

 HEADERS =	wrftranslator.h		\
		wrf_geometry.h		\
		wrf_gl_viewer.h		\
		wrf_ncl_viewer.h	\
		wrf_controller.h	\
		wrf_contour.h

#List of source files

 SOURCES =	wrftranslator.cpp	\
		wrf_geometry.cpp	\
		wrf_gl_viewer.cpp	\
		wrf_ncl_viewer.cpp	\
		wrf_controller.cpp	\
		wrf_contour.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../picking
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../windvector
 INCLUDEPATH	+=	../trajectory
 INCLUDEPATH	+=	../view
 INCLUDEPATH	+=	../marchingCube
 INCLUDEPATH	+=	../lic
 INCLUDEPATH	+=	../contour
 INCLUDEPATH	+=	../control
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../widget
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../stateboundary
 INCLUDEPATH	+=	../nclapp
 INCLUDEPATH	+=	../topography

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
 QMAKE_LIBDIR	+=	/opt/local/lib
#QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-lpicking
 LIBS		+=	-lgeometry
 LIBS		+=	-lview
 LIBS		+=	-lmarchingCube
 LIBS		+=	-lwindvector
 LIBS		+=	-ltrajectory
 LIBS		+=	-llic
 LIBS		+=	-lcontour
 LIBS		+=	-lcontrol
 LIBS		+=	-ltranslator
 LIBS		+=	-lcoastline
 LIBS		+=	-lstateboundary
 LIBS		+=	-lnclapp
 LIBS		+=	-ltopography
 LIBS		+=	-lwidget
 LIBS		+=	-lcairo
 LIBS		+=	-liconv.2.4.0

