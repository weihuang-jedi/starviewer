#$ID

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	marchTetrahedron.h

#List of source files

 SOURCES =	marchTetrahedron.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../shared

#List of extra needed libs:
 QMAKE_LIBDIR   +=	$(DESTDIR)
 LIBS		+=	-lshared

