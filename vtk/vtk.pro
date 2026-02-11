#$Id: vtk.pro 5322 2015-02-20 14:17:17Z starviewer $

! include( ../common.pri ) {
    error( Couldn't find the ../common.pri file! )
}

#CONFIG += staticlib 

 QMAKE_CXXFLAGS += -D_RSL4NV

#List of header files

 HEADERS =	vtkreader.h		\
		vtktranslator.h		\
		vtkcontroller.h		\
		vtk2dviewer.h		\
		vtk3dviewer.h

#List of source files

 SOURCES =	vtkreader.cpp		\
		vtktranslator.cpp	\
		vtkcontroller.cpp	\
		vtk2dviewer.cpp		\
		vtk3dviewer.cpp

#List of extra needed incs:

 INCLUDEPATH	+=	../include
 INCLUDEPATH	+=	../shared
 INCLUDEPATH	+=	../translator
 INCLUDEPATH	+=	../coastline
 INCLUDEPATH	+=	../marchTetrahedron
 INCLUDEPATH	+=	/Users/starviewer/ncl/src/paraview/src/VTK/IO/Legacy
 INCLUDEPATH	+=	/Users/starviewer/projects/ParaView-bin/VTK/IO/Legacy
 INCLUDEPATH	+=	/Users/starviewer/ncl/src/paraview/src/VTK/Common/ExecutionModel
 INCLUDEPATH	+=	/Users/starviewer/projects/ParaView-bin/VTK/Common/ExecutionModel
 INCLUDEPATH	+=	/Users/starviewer/ncl/src/paraview/src/VTK/Common/Core
 INCLUDEPATH	+=	/Users/starviewer/projects/ParaView-bin/VTK/Common/Core
 INCLUDEPATH	+=	/Users/starviewer/ncl/src/paraview/src/VTK/Common/DataModel
 INCLUDEPATH	+=	/Users/starviewer/projects/ParaView-bin/VTK/Common/DataModel

#List of extra needed libs:

 QMAKE_LIBDIR	+=	$(DESTDIR)
 QMAKE_LIBDIR	+=	/Users/starviewer/projects/ParaView-bin/lib

 LIBS		+=	-lshared
 LIBS		+=	-ltranslator
 LIBS		+=	-lcoastline
 LIBS		+=	-lmarchTetrahedron
 LIBS		+=	 /Users/starviewer/projects/ParaView-bin/lib/libvtkpqApplicationComponents-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkpqComponents-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkpqCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkpqWidgets-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkGUISupportQt-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonExecutionModel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonDataModel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonMath-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtksys-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonMisc-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonSystem-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonTransforms-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkInteractionStyle-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersExtraction-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersGeneral-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonComputationalGeometry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersStatistics-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingFourier-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkalglib-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersGeometry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersSources-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOImage-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkDICOMParser-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkzlib-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkmetaio-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkjpeg-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkpng-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtktiff-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOXMLParser-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkexpat-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingOpenGL-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingHybrid-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerManagerCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerImplementationCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVClientServerCoreCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersParallel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersModeling-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkParallelCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOLegacy-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVCommon-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkClientServer-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVVTKExtensionsCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkWrappingTools-pv4.0.a \
			 /Users/starviewer/projects/ParaView-bin/lib/libprotobuf.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libQtTesting.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerManagerApplication-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkChartsCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonColor-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkInfovisCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingContext2D-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingFreeType-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkfreetype-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkftgl-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkDomainsChemistry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOXML-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOGeometry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkjsoncpp-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersAMR-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersCosmo-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCosmo-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersFlowPaths-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersGeneric-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersHybrid-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingSources-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersHyperTree-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersParallelStatistics-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersProgrammable-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersTexture-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersVerdict-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkverdict-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOAMR-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkhdf5_hl-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkhdf5-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOEnSight-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOExodus-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkexoIIc-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkNetCDF-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkNetCDF_cxx-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOExport-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingGL2PS-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkgl2ps-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOImport-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOInfovis-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtklibxml2-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOMovie-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkoggtheora-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIONetCDF-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOPLY-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOParallel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOParallelExodus-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOParallelLSDyna-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOLSDyna-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOVPIC-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkVPIC-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOXdmf2-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkxdmf2-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingMorphological-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingGeneral-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkInteractionImage-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingColor-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkInteractionWidgets-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingAnnotation-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingVolume-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerManagerDefault-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerImplementationDefault-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVClientServerCoreDefault-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVClientServerCoreRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVVTKExtensionsRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingFreeTypeOpenGL-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingParallel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVVTKExtensionsWebGLExporter-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingLabel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingVolumeAMR-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingVolumeOpenGL-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkViewsContext2D-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkViewsCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVVTKExtensionsDefault-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerImplementationRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerManagerRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkTestingRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingHybridOpenGL-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingLOD-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVCatalyst-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVCatalystTestDriver-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkTestingGenericBridge-pv4.0.1.dylib \
			 -framework QtHelp -framework QtWebKit -framework QtXmlPatterns -framework QtSql \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOMovie-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkoggtheora-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersParallelStatistics-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOEnSight-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOImport-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOParallel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIONetCDF-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkNetCDF_cxx-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOParallelExodus-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOExodus-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkexoIIc-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkNetCDF-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerImplementationRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVClientServerCoreRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkDomainsChemistry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVVTKExtensionsRendering-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkChartsCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonColor-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersGeneric-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersHyperTree-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingFreeTypeOpenGL-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingParallel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVVTKExtensionsWebGLExporter-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOExport-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingGL2PS-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkgl2ps-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingLabel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingVolumeAMR-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingVolumeOpenGL-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkViewsContext2D-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingContext2D-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkViewsCore-pv4.0.1.dylib \
			 /Applications/Qt/Desktop/Qt/474/gcc/lib/libQtUiTools_debug.a \
			 -framework QtXml \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkGUISupportQt-pv4.0.1.dylib \
			 -framework QtGui -framework QtNetwork -framework QtCore \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersAMR-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkInfovisCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOXML-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOGeometry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkjsoncpp-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkhdf5_hl-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkhdf5-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtklibxml2-pv4.0.1.dylib \
			 -ldl -lm \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkInteractionWidgets-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkInteractionStyle-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersHybrid-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingGeneral-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingAnnotation-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingFreeType-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkftgl-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkfreetype-pv4.0.1.dylib \
			 -framework ApplicationServices -framework CoreServices \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingColor-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingVolume-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingOpenGL-pv4.0.1.dylib \
			 -framework AGL -framework OpenGL -framework ApplicationServices -framework IOKit -framework Cocoa \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingHybrid-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingSources-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVCatalyst-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerManagerApplication-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerManagerCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVServerImplementationCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVClientServerCoreCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersParallel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkRenderingCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersGeometry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOImage-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkDICOMParser-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkmetaio-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkpng-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtktiff-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkjpeg-pv4.0.1.dylib -lm \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersExtraction-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersStatistics-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingFourier-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkImagingCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkalglib-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersModeling-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersSources-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersGeneral-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonComputationalGeometry-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVVTKExtensionsCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkFiltersCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkParallelCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOLegacy-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkPVCommon-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOXMLParser-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkIOCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonExecutionModel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkzlib-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkexpat-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkClientServer-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libprotobuf.dylib \
			 /usr/lib/libz.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonDataModel-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonMisc-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonSystem-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonTransforms-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonMath-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtkCommonCore-pv4.0.1.dylib \
			 /Users/starviewer/projects/ParaView-bin/lib/libvtksys-pv4.0.1.dylib 

#/Users/starviewer/visweek/contest2014/reader/build/CMakeFiles/vtkStructuredGridReader.dir/link.txt

