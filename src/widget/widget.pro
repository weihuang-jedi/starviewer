#$ID

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	controlWidget.h		\
		displayWidget.h		\
		pixelWidget.h		\
		lightWidget.h		\
		licWidget.h		\
		vectorWidget.h		\
		trajectoryWidget.h	\
		clipWidget.h		\
		dataviewWidget.h	\
		isosurfaceWidget.h	\
		inspectorWidget.h	\
		subsetWidget.h		\
		minmaxWidget.h		\
		animationWidget.h

#List of source files

 SOURCES =	controlWidget.cpp	\
		displayWidget.cpp	\
		pixelWidget.cpp		\
		lightWidget.cpp		\
		licWidget.cpp		\
		vectorWidget.cpp	\
		clipWidget.cpp		\
		trajectoryWidget.cpp	\
		dataviewWidget.cpp	\
		isosurfaceWidget.cpp	\
		inspectorWidget.cpp	\
		subsetWidget.cpp	\
		minmaxWidget.cpp	\
		animationWidget.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../marchingCube
 INCLUDEPATH	+=	../nclapp
 INCLUDEPATH	+=	../view
 INCLUDEPATH	+=	../contour
 INCLUDEPATH	+=	../control
 INCLUDEPATH	+=	../translator

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
 QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lcoastline
 LIBS		+=	-lgeometry
 LIBS		+=	-lshared
 LIBS		+=	-lmarchingCube
 LIBS		+=	-lnclapp
 LIBS		+=	-lview
 LIBS		+=	-lcontour
 LIBS		+=	-lcontrol
 LIBS		+=	-ltranslator
 LIBS		+=	-liconv.2.4.0

