! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#DESTDIR = $$(NV_HOME)/bin
#message(The project will be installed in $$DESTDIR)

 TEMPLATE	= app

 QMAKE_CXXFLAGS += -D_RSL4NV
 QMAKE_CXXFLAGS += 
 QMAKE_LFLAGS   += 

 HEADERS	= mainwindow.h

 SOURCES	= mainwindow.cpp		\
		  main.cpp

#INSTALLS	+= target sources

 INCLUDEPATH	+= ../include
 INCLUDEPATH	+= ../shared
 INCLUDEPATH	+= ../marchTetrahedron
 INCLUDEPATH	+= ../marchingCube
 INCLUDEPATH	+= ../windvector
 INCLUDEPATH	+= ../trajectory
 INCLUDEPATH	+= ../lic
 INCLUDEPATH	+= ../geometry
 INCLUDEPATH	+= ../topography
 INCLUDEPATH	+= ../coastline
 INCLUDEPATH	+= ../coastline2
 INCLUDEPATH	+= ../stateboundary
 INCLUDEPATH	+= ../contour
 INCLUDEPATH	+= ../view
 INCLUDEPATH	+= ../control
 INCLUDEPATH	+= ../translator
 INCLUDEPATH	+= ../picking
 INCLUDEPATH	+= ../widget
 INCLUDEPATH	+= ../wrf
 INCLUDEPATH	+= ../mpas
 INCLUDEPATH	+= ../camse
 INCLUDEPATH	+= ../pop
 INCLUDEPATH	+= ../hdf
#INCLUDEPATH	+= ../radx
 INCLUDEPATH	+= ../test
 INCLUDEPATH	+= ../mpidemo
 INCLUDEPATH	+= ../nclapp

 QMAKE_LIBDIR	+= $(DESTDIR)
 QMAKE_LIBDIR	+= /opt/local/lib

 LIBS		+= -lshared
 LIBS		+= -lgeometry
 LIBS		+= -lcoastline
 LIBS		+= -lstateboundary
 LIBS		+= -lmarchTetrahedron
 LIBS		+= -lmarchingCube
 LIBS		+= -lwindvector
 LIBS		+= -ltrajectory
 LIBS		+= -llic
 LIBS		+= -lnclapp
 LIBS		+= -lcontour
 LIBS		+= -lview
 LIBS		+= -lcontrol
 LIBS		+= -ltranslator
 LIBS		+= -lpicking
 LIBS		+= -lwidget
 LIBS		+= -lwrf
 LIBS		+= -lmpas
 LIBS		+= -lcamse
 LIBS		+= -lpop
 LIBS		+= -lhdf
#LIBS		+= -lradx
 LIBS		+= -ltest
 LIBS		+= -lmpidemo
 LIBS		+= -llzma

