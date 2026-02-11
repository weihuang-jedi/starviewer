#include "vtkreader.h"

VTKReader::VTKReader(string flnm)
{
    inputFilename = flnm;
    reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

    polyData = NULL;
    ispolydata = reader->IsFilePolyData();

    cout << "reader->IsFileStructuredPoints() = " << reader->IsFileStructuredPoints() << endl;

    if(ispolydata)
    {
        cout << "\nfile: " << __FILE__;
        cout << "\tline: " << __LINE__;
        cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
        cout << "polyData is a polydata" << endl;

        polyData = reader->GetPolyDataOutput();
        polyData->BuildCells();
      //polyData->BuildLinks();
      //polyData->SetOutput(polyData);
      //reader->SetInputData(polyData);

#if 0
        cout << "polyData has " << polyData->GetNumberOfPoints() << " points." << endl;
        cout << "polyData has " << polyData->GetNumberOfCells() << " cells." << endl;
      //cout << "polyData has " << polyData->GetNumberOfVerts() << " verts." << endl;
      //cout << "polyData has " << polyData->GetNumberOfLines() << " lines." << endl;
        cout << "polyData has " << polyData->GetNumberOfPolys() << " polys." << endl;
      //cout << "polyData has " << polyData->GetNumberOfStrips() << " strips." << endl;

      //polyData->BuildLinks();

      //vtkCellArray *cellArray = polyData->GetVerts();
      //vtkCellArray *lineArray = polyData->GetLines();

        polyArray = polyData->GetPolys();
      //polyArray->PrintSelf(ostream& os, vtkIndent indent);
      //polyArray->InitTraversal();
      //vtkIdType *pntrs = polyArray->GetPointer();
        cout << "polyArray has size " << polyArray->GetSize() << "." << endl;

      //vtkIdType loc;
        vtkIdType npts;
        vtkIdType *pntrs = (vtkIdType *)calloc(10, sizeof(vtkIdType));
        polyArray->GetCell(0, npts, pntrs);

        vtkCell *cell = polyData->GetCell(0);
        vtkDataArray *dataArray = cell->Points[0].GetData();

        cout << "cell has " << cell->GetNumberOfPoints() << " points." << endl;
        cout << "dataArray has " << dataArray->GetNumberOfTuples() << " tuples." << endl;
        cout << "dataArray has " << dataArray->GetNumberOfComponents() << " components." << endl;

        int i, j;

        for(j = 0; j < dataArray->GetNumberOfTuples(); ++j)
        {
            cout << "Tuple(" << j << ", *) = (";
            for(i = 0; i < dataArray->GetNumberOfComponents(); ++i)
            {
                if(i)
                    cout << ", ";
                cout << dataArray->GetComponent(j, i);
            }
            cout << ")" << endl;
        }

        cout << "polyArray has size " << polyArray->GetNumberOfConnectivityEntries() << "." << endl;
#endif
    }
}

VTKReader::~VTKReader()
{
}

