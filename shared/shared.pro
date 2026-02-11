#$Id: shared.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Could not find the ../common.pri file! )
}

#List of header files

 HEADERS	=	ball.h		\
			clip.h		\
			color.h		\
			cone.h		\
			cylinder.h	\
			equalcylinder.h	\
			earth.h		\
			light.h		\
			util.h		\
			lister.h	\
			locator.h	\
			texture1d.h	\
			texture3d.h	\
			texturekeeper.h	\
			colorTable.h	\
			evaluator.h	\
			nvoptions.h	\
			niceminmaxstepsize.h	\
			sliderNspin.h

#List of source files
#			gcinout.f	\

 SOURCES	=	ball.cpp	\
			clip.cpp	\
			color.cpp	\
			cone.cpp	\
			cylinder.cpp	\
			equalcylinder.cpp	\
			earth.cpp	\
			light.cpp	\
			lister.cpp	\
			locator.cpp	\
			texture1d.cpp	\
			texture3d.cpp	\
			texturekeeper.cpp	\
			colorTable.cpp	\
			evaluator.cpp	\
			nvoptions.cpp	\
			niceminmaxstepsize.cpp	\
			sliderNspin.cpp

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../geometry

 QMAKE_LIBDIR   += $(DESTDIR)

#QMAKE_RPATHDIR += /Users/weihuang/lib/NCLDEV/lib

 QMAKE_LIBDIR   += /Users/weihuang/lib/NCLDEV/lib
 QMAKE_LIBDIR   += /opt/local/lib/gcc48

 LIBS		+= -lgfortran
 LIBS		+= -lquadmath
 LIBS		+= -lgeometry
 LIBS		+= -lnfpfort
 LIBS		+= -lngmath
 LIBS		+= -lncarg

