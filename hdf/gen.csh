#!/bin/csh

 mv pop.pro hdf.pro

 foreach f ( 2dviewer 3dviewer controller reader translator )
     mv pop${f}.cpp hdf${f}.cpp
     mv pop${f}.h   hdf${f}.h
 end

