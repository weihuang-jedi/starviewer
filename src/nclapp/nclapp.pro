# $Id
# nclapp files
# Wei Huang

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#TEMPLATE	=	app

 HEADERS	=	attribute.h		\
			dimension.h		\
			variable.h		\
			nclfilehandler.h	\
			nvFile.h

 SOURCES	=	attribute.cpp		\
			dimension.cpp		\
			variable.cpp		\
			nclfilehandler.cpp	\
			nvFile.cpp

#List of extra needed incs:

 INCLUDEPATH	+=      ../include
 INCLUDEPATH	+=      ../shared

#List of extra needed libs:

 QMAKE_LIBDIR	+= $(DESTDIR)
#QMAKE_LIBDIR   += /usr/lib
 QMAKE_LIBDIR   += /Users/weihuang/lib/NCLDEV/lib
 QMAKE_LIBDIR   += /usr/local/lib
 QMAKE_LIBDIR   += /opt/local/lib
 QMAKE_LIBDIR   += /opt/X11/lib
 QMAKE_LIBDIR   += /opt/local/lib/gcc48
 QMAKE_LIBDIR   += /opt/local/lib/gcc48/gcc/x86_64-apple-darwin14/4.8.5

 LIBS	+=	-lshared
 LIBS	+=	-lngmath
 LIBS	+=	-lncarg
 LIBS	+=	-lhlu
 LIBS	+=	-lncl
 LIBS	+=	-lnclapi
 LIBS   +=      -lnetcdf
 LIBS   +=      -lhdf5_hl
 LIBS   +=      -lhdf5
 LIBS   +=      -lz -lm
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
 LIBS   +=      -liconv.2.4.0
 LIBS   +=      -lgrib2c -lgdal -lproj
 LIBS   +=      -lXrender -lXpm -lX11 -lXext
 LIBS   +=      -lfreetype
 LIBS   +=      -lgfortran -lquadmath -lgcc
 LIBS	+=	-Wl,-no_compact_unwind

