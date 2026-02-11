#$ID

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	arrow.h		\
		windvector.h

#List of source files

 SOURCES =	arrow.cpp	\
		windvector.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
#INCLUDEPATH	+=	../coastline
#INCLUDEPATH	+=	../geometry
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../topography
#INCLUDEPATH	+=	../nclapp
#INCLUDEPATH	+=	../control

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
 QMAKE_LIBDIR	+=	/usr/lib

#LIBS		+=	-lcoastline
#LIBS		+=	-lgeometry
 LIBS		+=	-lshared
 LIBS		+=	-ltopography
#LIBS		+=	-lnclapp
#LIBS		+=	-lcontrol
 LIBS		+=	-liconv.2.4.0

