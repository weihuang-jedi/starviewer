#$Id: test.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 
#QMAKE_CXXFLAGS += -fopenmp
#QMAKE_LFLAGS   += -fopenmp

#List of header files

 HEADERS =	testtranslator.h

#List of source files

 SOURCES =	testtranslator.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../topography
 INCLUDEPATH	+=	../coastline2
 INCLUDEPATH	+=	../nclapp

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)

 LIBS		+=	-lshared
 LIBS		+=	-ltranslator
 LIBS		+=	-ltopography
 LIBS		+=	-lcoastline2
 LIBS		+=	-lnclapp

