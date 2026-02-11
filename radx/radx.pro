#$Id: radx.pro 5321 2015-02-20 03:20:52Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

 QMAKE_CXXFLAGS += -D_RSL4NV

#List of header files

 HEADERS =	radxreader.h		\
		radxtranslator.h	\
		radxcontroller.h	\
		radx2dviewer.h		\
		radx3dviewer.h

#List of source files

 SOURCES =	radxreader.cpp		\
		radxtranslator.cpp	\
		radxcontroller.cpp	\
		radx2dviewer.cpp	\
		radx3dviewer.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../coastline2
 INCLUDEPATH	+=	../marchTetrahedron

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
 QMAKE_LIBDIR	+=	/usr/lib

 LIBS		+=	-lshared
 LIBS		+=	-ltranslator
 LIBS		+=	-lcoastline2
 LIBS		+=	-lmarchTetrahedron
 LIBS		+=	-lRadx -lnetcdf_c++ -lhdf5_cpp -lhdf5_hl_cpp
 LIBS		+=	-liconv.2.4.0

