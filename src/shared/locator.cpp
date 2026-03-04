// $Id: locator.cpp 4833 2013-11-05 15:49:31Z starviewer $

#include "locator.h"

Locator::Locator()
{
    _x = 0;
    _y = 0;
    _y = 0;

    _on = false;

    _distance = 8.0;
}

Locator::Locator(int x, int y, int z)
{
    _x = x;
    _y = y;
    _y = z;

    _on = false;
}

Locator::~Locator()
{
}

int Locator::pinout(double plat, double plon,
                    double* qlat, double* qlon, int npts)
{
    int n;
    int left, rite, top, low;
    double* work = NULL;

    left = 0;
    rite = 0;
    left = 0;
    rite = 0;

    for(n = 0; n < npts; ++n)
    {
        if(plon < qlon[n])
            ++left;
        if(plon > qlon[n])
            ++rite;
        if(plat < qlat[n])
            ++low;
        if(plat > qlat[n])
            ++top;
    }

    if((npts == left) || (npts == rite) ||
       (npts == top ) || (npts == low ))
        return 0;

    work = (double*) calloc(4*(npts+1), sizeof(double));
    assert(work);

    qlat[npts] = qlat[0];
    qlon[npts] = qlon[0];

    n = npts + 1;
    return GCINOUT(&plat,&plon, qlat, qlon, &n, work);
}

