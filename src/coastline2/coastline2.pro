#$ID

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#List of header files

 HEADERS =	coastline2.h

#List of source files

 SOURCES =	coastline2.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared

#List of extra needed libs:

 QMAKE_LIBDIR	+= $(DESTDIR)
 QMAKE_LIBDIR   += /opt/local/lib
 QMAKE_LIBDIR   += /opt/X11/lib
 QMAKE_LIBDIR   += /opt/local/lib/gcc48
 QMAKE_LIBDIR   += /opt/local/lib/gcc48/gcc/x86_64-apple-darwin14/4.8.5
 QMAKE_LIBDIR   += /usr/local/lib
 QMAKE_LIBDIR	+= /usr/lib
 QMAKE_LIBDIR   += /Users/weihuang/lib/NCLDEV/lib

 LIBS	+=	-lshared
 LIBS	+=	-lngmath
 LIBS	+=	-lncarg
 LIBS	+=	-lhlu
 LIBS	+=	-lncl
 LIBS	+=	-lnclapi
 LIBS   +=      -lnetcdf
 LIBS   +=      -lhdf5_hl
 LIBS   +=      -lhdf5
 LIBS   +=      -lz
 LIBS   +=      -lsz
 LIBS   +=      -lhdfeos
 LIBS   +=      -lhe5_hdfeos
 LIBS   +=      -lGctp
 LIBS   +=      -lmfhdf
 LIBS   +=      -ldf
 LIBS   +=      -lsphere3.1_dp
 LIBS   +=      -lfftpack5_dp
 LIBS   +=      -llapack_ncl
 LIBS   +=      -lblas_ncl
 LIBS   +=      -lnfp
 LIBS   +=      -lnfpfort
 LIBS   +=      -lncarg_gks
 LIBS   +=      -lncarg_c
 LIBS   +=      -ludunits2
 LIBS   +=      -lpng -ljpeg -ljasper
 LIBS   +=      -lcairo
 LIBS   +=      -lgrib2c -lgdal -lproj
 LIBS   +=      -lXrender -lXpm -lX11 -lXext
 LIBS   +=      -lfreetype
 LIBS   +=      -lgfortran -lquadmath -lgcc
 LIBS	+=	-liconv.2.4.0
 LIBS	+=	-Wl,-no_compact_unwind



