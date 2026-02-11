#$Id: geometry.pro 5314 2015-02-06 20:58:52Z starviewer $

! include( ../common.pri ) {
    error( Could not find the ../common.pri file! )
}

#QT -= gui

#List of header files

 HEADERS	=	basegeometry.h		\
			geometry.h

#List of source files

 SOURCES	=	basegeometry.cpp	\
			geometry.cpp

