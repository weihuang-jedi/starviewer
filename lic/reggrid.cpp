#include "reggrid.h"

RegGrid::RegGrid(ColorTable* ct)
{
    colorTable = ct;
    maxsteps = MAXSTEPS;
}

RegGrid::~RegGrid()
{
    int j;
    for(j = 0; j < ny; ++j)
    {
        free(vecdata[j]);
        free(hitdata[j]);
        free(texdata[j]);
        free(Idata[j]);
    }

    free(vecdata);
    free(hitdata);
    free(texdata);
    free(Idata);
}

void RegGrid::setup(int ix, int iy)
{
    int i, j;

    nx = ix;
    ny = iy;

    vecdata = (LICvector **) malloc(ny * sizeof(LICvector *));
    hitdata = (int **) malloc(ny * sizeof(int *));
    texdata = (int **) malloc(ny * sizeof(int *));
    Idata   = (double **) malloc(ny * sizeof(double *));

  //vecdata = new LICvector *[ny];
  //hitdata = new int *[ny];
  //texdata = new int *[ny];
  //Idata = new double *[ny];

    numvalid=0;

    for(j = 0; j < ny; ++j)
    {
        vecdata[j] = new LICvector[nx];
        hitdata[j] = new int[nx];
        texdata[j] = new int[nx];
        Idata[j]   = new double[nx];

        for(i = 0; i < nx; ++i)
        {
            hitdata[j][i] = 0;
            Idata[j][i] = 0.0;
            texdata[j][i] = 0;
        }
    }
}

int RegGrid::getPixel(const Point p, int &i, int &j)
{
    i = (int)p.coord[0];
    j = (int)p.coord[1];

    if((i >= 0) && (i < nx) && (j >= 0) && (j < ny))
        return 1;
    else
        return 0;
 }

double RegGrid::ComputeI(StreamLine &s, int &numvalid)
{
    double T,I;
    int i,j;
    int L = get_maxsteps();

    T=0.0;
    numvalid = 0;
    
    for(i = -L; i <= L; ++i)
    {
        if(validpt(s[i]))
        {
	    T += getT(s[i]);
	    numvalid++;
        }
    }

    if(getPixel(s[0],i,j))
    {
        I = T / numvalid;
        Idata[j][i] += I;
        ++hitdata[j][i];
        return I;
    }

    return 0.0;
}

double RegGrid::ComputeIFwd(StreamLine &s, double &I, int m, int &numvalid)
{
    int i,j;
    double k;
    int L = get_maxsteps();

    if(getPixel(s[m],i,j))
    {
        if(validpt(s[m+L]))
	    numvalid++;
        if(validpt(s[m-1-L]))
	    numvalid--;

        k = 1./numvalid;

        Idata[j][i] += I += k*(getT(s[m+L]) - getT(s[m-1-L]));
        hitdata[j][i]++;
        return I;
    }

    return 0;
}

double RegGrid::ComputeIBwd(StreamLine &s, double &I, int m, int &numvalid)
{
    int i,j;
    double k;
    int L = get_maxsteps();
    
    if(getPixel(s[m],i,j))
    {
        if(validpt(s[m-L]))
	    numvalid++;
        if(validpt(s[m+1+L]))
	    numvalid--;

        k = 1./numvalid;
        I += (k*(getT(s[m-L]) - getT(s[m+1+L])));
        Idata[j][i] += I;
        ++hitdata[j][i];

        return I;
    }

    return 0.0;
}
  
double RegGrid::getT(Point &p)
{
    int i,j;

    if(getPixel(p,i,j)) 
        return texdata[j][i];
    return 0.0;
}

LICvector RegGrid::getLICvector(const Point &p)
{
    int i,j;

    if(getPixel(p,i,j))
    {
        return vecdata[j][i];
    }

    return(LICvector(0,0,0));
}

void RegGrid::Normalize()
{
    int i, j;

    for(j = 0; j < ny; ++j)
    {
        for(i = 0; i < nx; ++i)
        {
            Idata[j][i] /= hitdata[j][i];
        }
    }
}

int RegGrid::validpt(Point &p)
{
    int i,j;

    if(getPixel(p,i,j))
        return 1;
    else
        return 0;
}
  
void RegGrid::Print()
{
    int i, j;
  //double scale;
    double magind;
    double mag;
  //double color[4];
    
    printf("P3\t%d\t%d\t255\n",nx,ny);
    for(j = 0; j < ny; ++j)
    {
        for(i = 0; i < nx; ++i)
        {
	  //scale = Idata[j][i]/255.;
	    mag = vecdata[j][i].size();
	    magind = mag/maxvecmag;
            printf("magind:%lf\n",magind);
	  //color = (*table)[magind]*scale;
          //printf("(%d, %d) = %d: {%f %f %f}\n", j, i, (int)Idata[j][i],
          //        color[0], color[1], color[2]);
        }
    }
}

