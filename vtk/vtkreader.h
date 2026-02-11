#ifndef _vtkreader_h_
#define _vtkreader_h_

#include <string>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkGenericDataObjectReader.h>

using namespace std;

class VTKReader
{
    public:
        VTKReader(string flnm);
       ~VTKReader();

        vtkPolyData* getPolyData() { return polyData; };

    protected:
        string inputFilename;
        vtkSmartPointer<vtkGenericDataObjectReader> reader;
        vtkPolyData* polyData;
        bool ispolydata;
};
#endif

