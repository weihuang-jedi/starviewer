#DESTDIR = /Users/huangwei/work/nv
 DESTDIR = /usr/local
#DESTDIR = /glade/u/apps/contrib/nv

 NCL_DIR = /Users/huangwei/ncl/NCLDEV/lib

#message(The project will be installed in $$DESTDIR)

 TEMPLATE	= subdirs

 CONFIG		+= ordered
#CONFIG		+= debug

#main must be last
 SUBDIRS	+= geometry
 SUBDIRS	+= shared
 SUBDIRS	+= nclapp
 SUBDIRS	+= coastline
 SUBDIRS	+= coastline2
 SUBDIRS	+= stateboundary
 SUBDIRS	+= topography
 SUBDIRS	+= picking
 SUBDIRS	+= contour
 SUBDIRS	+= marchingCube
 SUBDIRS	+= marchTetrahedron
 SUBDIRS	+= view
 SUBDIRS	+= control
 SUBDIRS	+= translator
 SUBDIRS	+= widget
 SUBDIRS	+= windvector
 SUBDIRS	+= trajectory
 SUBDIRS	+= lic
 SUBDIRS	+= wrf
 SUBDIRS	+= mpas
 SUBDIRS	+= camse
 SUBDIRS	+= pop
 SUBDIRS	+= hdf
 SUBDIRS	+= test
 SUBDIRS	+= mpidemo
#SUBDIRS	+= radx
#SUBDIRS	+= vtk
 SUBDIRS	+= main

