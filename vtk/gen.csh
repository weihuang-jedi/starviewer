#!/bin/csh

 mv radar.pro vtk.pro

 foreach f ( 2dviewer 3dviewer controller reader translator )
     mv radar${f}.cpp vtk${f}.cpp
     mv radar${f}.h   vtk${f}.h
 end

