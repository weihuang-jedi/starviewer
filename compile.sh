#set echo

#os=`uname -s`

 myqmake

 for dir in [ \
 		geometry \
 		shared	\
 		nclapp	\
 		coastline \
 		coastline2 \
 		topography \
 		picking	\
 		contour	\
 		marchingCube	\
 		marchTetrahedron	\
 		view	\
 		control	\
 		translator	\
 		wrf	\
 		mpas	\
 		camse	\
 		pop	\
 		hdf	\
 		test	\
 		mpidemo	\
 		widget	\
 		windvector	\
 		trajectory	\
 		lic	\
 		main ]
 do
     cd $dir

     myqmake

     make

     cd ..
 done

 make

