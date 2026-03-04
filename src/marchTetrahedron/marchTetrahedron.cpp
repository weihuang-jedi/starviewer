//
// Marching Cubes Example Program 
// by Cory Bloyd (corysama@yahoo.com)
//
// A simple, portable and complete implementation of the Marching Cubes
// and Marching Tetrahedrons algorithms in a single source file.
// There are many ways that this code could be made faster, but the 
// intent is for the code to be easy to understand.
//
// For a description of the algorithm go to
// http://astronomy.swin.edu.au/pbourke/modelling/polygonise/
//
// This code is public domain.
//

#include "marchTetrahedron.h"

float fGetOffset(float fValue1, float fValue2, float fValueDesired);

//These tables are used so that everything can be done in little loops
// that you can look at all at once
// rather than in pages and pages of unrolled code.

//a2iTetrahedronEdgeConnection lists the index of the endpoint vertices
//for each of the 6 edges of the tetrahedron
static const int a2iTetrahedronEdgeConnection[6][2] =
{
        {0,1},  {1,2},  {2,0},  {0,3},  {1,3},  {2,3}
};

//a2iTetrahedronEdgeConnection lists the index of verticies from a cube 
// that made up each of the six tetrahedrons within the cube
//static const int a2iTetrahedronsInACube[6][4] =
//{
//        {0,5,1,6},
//        {0,1,2,6},
//        {0,2,3,6},
//        {0,3,7,6},
//        {0,7,4,6},
//        {0,4,5,6},
//};

static const int a2iTetrahedronsInACube[6][4] =
{
        {0,5,1,6},
        {0,1,2,6},
        {0,4,5,6},
        {0,5,1,6},
        {0,1,2,6},
        {0,4,5,6}
};

//For any edge, if one vertex is inside of the surface
//and the other is outside of the surface
//then the edge intersects the surface
//For each of the 4 vertices of the tetrahedron
//can be two possible states : either inside or outside of the surface
//For any tetrahedron the are 2^4=16 possible sets of vertex states
//This table lists the edges intersected by the surface for all 16
//possible vertex states
//There are 6 edges.
//For each entry in the table,
//if edge #n is intersected, then bit #n is set to 1
static const int aiTetrahedronEdgeFlags[16] = 
{
    0x00, 0x0d, 0x13, 0x1e, 0x26, 0x2b, 0x35, 0x38,
    0x38, 0x35, 0x2b, 0x26, 0x1e, 0x13, 0x0d, 0x00,
};

// For each of the possible vertex states listed
// in aiTetrahedronEdgeFlags there is a specific triangulation
// of the edge intersection points.
// a2iTetrahedronTriangles lists all of them in the form of
// 0-2 edge triples with the list terminated by the invalid value -1.
//
// I generated this table by hand

int a2iTetrahedronTriangles[16][7] =
{
        {-1, -1, -1, -1, -1, -1, -1},
        { 0,  3,  2, -1, -1, -1, -1},
        { 0,  1,  4, -1, -1, -1, -1},
        { 1,  4,  2,  2,  4,  3, -1},

        { 1,  2,  5, -1, -1, -1, -1},
        { 0,  3,  5,  0,  5,  1, -1},
        { 0,  2,  5,  0,  5,  4, -1},
        { 5,  4,  3, -1, -1, -1, -1},

        { 3,  4,  5, -1, -1, -1, -1},
        { 4,  5,  0,  5,  2,  0, -1},
        { 1,  5,  0,  5,  3,  0, -1},
        { 5,  2,  1, -1, -1, -1, -1},

        { 3,  4,  2,  2,  4,  1, -1},
        { 4,  1,  0, -1, -1, -1, -1},
        { 2,  3,  0, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1},
};

void get_norm(pt3d &asNorm, pt3d asTriangle[],
              pt3d asEdgeVector, float descend)
{
    float dot_product;
    float x1 = asTriangle[1].fX - asTriangle[0].fX;
    float y1 = asTriangle[1].fY - asTriangle[0].fY;
    float z1 = asTriangle[1].fZ - asTriangle[0].fZ;

    float x2 = asTriangle[2].fX - asTriangle[0].fX;
    float y2 = asTriangle[2].fY - asTriangle[0].fY;
    float z2 = asTriangle[2].fZ - asTriangle[0].fZ;

    asNorm.fX = y1 * z2 - y2 * z1;
    asNorm.fY = z1 * x2 - z2 * x1;
    asNorm.fZ = x1 * y2 - x2 * y1;

    dot_product = asNorm.fX * asEdgeVector.fX
                + asNorm.fY * asEdgeVector.fY
                + asNorm.fZ * asEdgeVector.fZ;

    if(0 < dot_product * descend)
    {
        asNorm.fX = - asNorm.fX;
        asNorm.fY = - asNorm.fZ;
        asNorm.fZ = - asNorm.fZ;
    }
}

void get_norm2(pt3d &asNorm, pt3d asTriangle[])
{
    float x1 = asTriangle[1].fX - asTriangle[0].fX;
    float y1 = asTriangle[1].fY - asTriangle[0].fY;
    float z1 = asTriangle[1].fZ - asTriangle[0].fZ;

    float x2 = asTriangle[2].fX - asTriangle[0].fX;
    float y2 = asTriangle[2].fY - asTriangle[0].fY;
    float z2 = asTriangle[2].fZ - asTriangle[0].fZ;

    asNorm.fX = y1 * z2 - y2 * z1;
    asNorm.fY = z1 * x2 - z2 * x1;
    asNorm.fZ = x1 * y2 - x2 * y1;
}

MarchTetrahedron::MarchTetrahedron()
{
}

MarchTetrahedron::~MarchTetrahedron()
{
}

//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
float fGetOffset(float fValue1, float fValue2, float fValueDesired)
{
    GLdouble fDelta = fValue2 - fValue1;

    if(fDelta == 0.0)
    {
        return 0.5;
    }
    return (fValueDesired - fValue1)/fDelta;
}

void MarchTetrahedron::vTetrahedron(pt3d *pasTetrahedronPosition,
                                    float *pafTetrahedronValue)
{
     int n;

     for(n = 0; n < isolevel; ++n)
     {
         if(n >= colorlen)
             break;

         glColor4fv(colormap[n]);

         _runTetrahedron(pasTetrahedronPosition,
                         pafTetrahedronValue,
                         isovalue[n]);
     }
}

//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
void MarchTetrahedron::_runTetrahedron(pt3d *pasTetrahedronPosition,
                                       float *pafTetrahedronValue,
                                       float fTargetValue)
{
    int iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
    float fOffset, fInvOffset;
    pt3d asEdgeVertex[6];
    pt3d asTriangle[3];
    pt3d asNorm;

  //Find which vertices are inside of the surface and which are outside
    for(iVertex = 0; iVertex < 4; iVertex++)
    {
        if(pafTetrahedronValue[iVertex] <= fTargetValue) 
            iFlagIndex |= 1<<iVertex;
    }

  //Find which edges are intersected by the surface
    iEdgeFlags = aiTetrahedronEdgeFlags[iFlagIndex];

  //If the tetrahedron is entirely inside or outside of the surface,
  //then there will be no intersections
    if(iEdgeFlags == 0)
        return;

  //Find the point of intersection of the surface with each edge
  //Then find the normal to the surface at those points
    for(iEdge = 0; iEdge < 6; iEdge++)
    {
      //if there is an intersection on this edge
        if(iEdgeFlags & (1<<iEdge))
        {
            iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
            iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
            fOffset = fGetOffset(pafTetrahedronValue[iVert0],
                                 pafTetrahedronValue[iVert1],
                                 fTargetValue);
            fInvOffset = 1.0 - fOffset;

            asEdgeVertex[iEdge].fX = fInvOffset*pasTetrahedronPosition[iVert0].fX
                                     +  fOffset*pasTetrahedronPosition[iVert1].fX;
            asEdgeVertex[iEdge].fY = fInvOffset*pasTetrahedronPosition[iVert0].fY
                                     +  fOffset*pasTetrahedronPosition[iVert1].fY;
            asEdgeVertex[iEdge].fZ = fInvOffset*pasTetrahedronPosition[iVert0].fZ
                                     +  fOffset*pasTetrahedronPosition[iVert1].fZ;
        }
    }

    glBegin(GL_TRIANGLES);
  //Draw the triangles that were found.  There can be up to 2 per tetrahedron
    for(iTriangle = 0; iTriangle < 2; iTriangle++)
    {
        if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)
            break;

        for(iCorner = 0; iCorner < 3; iCorner++)
        {
            iVertex = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+iCorner];

            asTriangle[iCorner] = asEdgeVertex[iVertex];
        }

        get_norm2(asNorm, asTriangle);
        glNormal3f(asNorm.fX, asNorm.fY, asNorm.fZ);

        for(iCorner = 0; iCorner < 3; iCorner++)
        {
            glVertex3f(asTriangle[iCorner].fX,
                       asTriangle[iCorner].fY,
                       asTriangle[iCorner].fZ);
        }
    }
    glEnd();
}

