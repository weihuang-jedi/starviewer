#$Id: mpidemo.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

#List of header files

 HEADERS =	mpicube.h		\
		mpicolor.h		\
		mpiviewer.h		\
		mpiprocessor.cpp	\
		mpitranslator.h

#List of source files

 SOURCES =	mpicube.cpp		\
		mpicolor.cpp		\
		mpiviewer.cpp		\
		mpiprocessor.cpp	\
		mpitranslator.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../translator

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)

 LIBS		+=	-lshared
 LIBS		+=	-ltranslator

