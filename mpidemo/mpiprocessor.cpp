//$Id: mpiprocessor.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include "mpiprocessor.h"

MPIProcessor::MPIProcessor()
{
    c4v = new float[4];
    xc4v = new float[4];
    yc4v = new float[4];
    mpi_color = new MPIColor();
    cube = new MPICube();

    halo_width = 0;
    numb_procs = 1;

    zStart.x = 0;
    zStart.y = 0;
    zStart.z = 0;
}

MPIProcessor::~MPIProcessor()
{
    delete c4v;
    delete xc4v;
    delete yc4v;
    delete mpi_color;
    delete cube;
}

void MPIProcessor::draw_with_halo_filled()
{
    int neighbor_rank;
    float s = 0.50;
    pointstruct start, size;

  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;

    glPushMatrix();

    glScalef(s, s, s);
  //cube->set_alpha(1.0);

    shift.x = 6.0 * (zRank.col - zPlane.col/2) * delta;
    shift.y = 6.0 * (zRank.row - zPlane.row/2) * delta;
    shift.z = 0.0;

  //Draw the center
    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v((ColorName)myrank);
    cube->set_cubecolor(c4v);

    drawAregion(zStart, zSize, shift);

  //Draw the halo region
    c4v = mpi_color->get_color4v(COLOR_SKYBLUE);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

  //Draw Left
    if(zRank.col)
        c4v = mpi_color->get_color4v((ColorName) (myrank - 1));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x - halo_width;
    start.y = zStart.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = zSize.y;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw right
    if(zRank.col != (zPlane.col - 1))
        c4v = mpi_color->get_color4v((ColorName) (myrank + 1));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x + zSize.x;
    start.y = zStart.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = zSize.y;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the bottom-left
    neighbor_rank = (zRank.row - 1) * zPlane.col + zRank.col - 1;

  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tzRank.col = " << zRank.col << ", zPlane.col = " << zPlane.col << endl;
  //cout << "\tzRank.row = " << zRank.row << ", zPlane.row = " << zPlane.row << endl;
  //cout << "\tmyrank = " << myrank << ", neighbor_rank = " << neighbor_rank << endl;

    if((0 <= neighbor_rank) && zRank.col)
        c4v = mpi_color->get_color4v((ColorName) (neighbor_rank));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x - halo_width;
    start.y = zStart.y - halo_width;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the bottom-center
    ++neighbor_rank;
    if(0 <= neighbor_rank)
        c4v = mpi_color->get_color4v((ColorName) (neighbor_rank));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x;
    start.y = zStart.y - halo_width;
    start.z = zStart.z;

    size.x = zSize.x;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the bottom-right
    ++neighbor_rank;
    if((0 <= neighbor_rank) && (zRank.col != (zPlane.col - 1)))
        c4v = mpi_color->get_color4v((ColorName) (neighbor_rank));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x + zSize.x;
    start.y = zStart.y - halo_width;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the top-left
    neighbor_rank = (zRank.row + 1) * zPlane.col + zRank.col - 1;
    if((numb_procs > neighbor_rank) && zRank.col)
        c4v = mpi_color->get_color4v((ColorName) (neighbor_rank));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x - halo_width;
    start.y = zStart.y + zSize.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the top-center
   ++neighbor_rank;
    if(numb_procs > neighbor_rank)
        c4v = mpi_color->get_color4v((ColorName) (neighbor_rank));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x;
    start.y = zStart.y + zSize.y;
    start.z = zStart.z;

    size.x = zSize.x;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the top-right
   ++neighbor_rank;
    if((numb_procs > neighbor_rank) && (zRank.col != (zPlane.col - 1)))
        c4v = mpi_color->get_color4v((ColorName) (neighbor_rank));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x + zSize.x;
    start.y = zStart.y + zSize.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

    glPopMatrix();
}

void MPIProcessor::draw_row_exchanged()
{
    float s = 0.50;
    pointstruct start, size;

    glPushMatrix();

    glScalef(s, s, s);
  //cube->set_alpha(1.0);

    shift.x = 6.0 * (zRank.col - zPlane.col/2) * delta;
    shift.y = 6.0 * (zRank.row - zPlane.row/2) * delta;
    shift.z = 0.0;

  //Draw the center
    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v((ColorName)myrank);
    cube->set_cubecolor(c4v);

    drawAregion(zStart, zSize, shift);

  //Draw the halo region
    c4v = mpi_color->get_color4v(COLOR_SKYBLUE);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

  //Draw the bottom
    start.x = zStart.x - halo_width;
    start.y = zStart.y - halo_width;
    start.z = zStart.z;

    size.x = zSize.x + 2*halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the top
    start.x = zStart.x - halo_width;
    start.y = zStart.y + zSize.y;
    start.z = zStart.z;

    size.x = zSize.x + 2*halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw Left
    if(zRank.col)
        c4v = mpi_color->get_color4v((ColorName) (myrank - 1));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x - halo_width;
    start.y = zStart.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = zSize.y;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw right
    if(zRank.col != (zPlane.col - 1))
        c4v = mpi_color->get_color4v((ColorName) (myrank + 1));
    else
        c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

    start.x = zStart.x + zSize.x;
    start.y = zStart.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = zSize.y;
    size.z = zSize.z;

    drawAregion(start, size, shift);

    glPopMatrix();
}

void MPIProcessor::draw_with_halo()
{
    float s = 0.50;
    pointstruct start, size;

    glPushMatrix();

    glScalef(s, s, s);
  //cube->set_alpha(1.0);

    shift.x = 6.0 * (zRank.col - zPlane.col/2) * delta;
    shift.y = 6.0 * (zRank.row - zPlane.row/2) * delta;
    shift.z = 0.0;

  //Draw the center
    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v((ColorName)myrank);
    cube->set_cubecolor(c4v);

    drawAregion(zStart, zSize, shift);


  //Draw the halo region
    c4v = mpi_color->get_color4v(COLOR_SKYBLUE);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v(COLOR_LIGHTGRAY);
    cube->set_cubecolor(c4v);

  //Draw the bottom
    start.x = zStart.x - halo_width;
    start.y = zStart.y - halo_width;
    start.z = zStart.z;

    size.x = zSize.x + 2*halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw the top
    start.x = zStart.x - halo_width;
    start.y = zStart.y + zSize.y;
    start.z = zStart.z;

    size.x = zSize.x + 2*halo_width;
    size.y = halo_width;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw Left
    start.x = zStart.x - halo_width;
    start.y = zStart.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = zSize.y;
    size.z = zSize.z;

    drawAregion(start, size, shift);

  //Draw right
    start.x = zStart.x + zSize.x;
    start.y = zStart.y;
    start.z = zStart.z;

    size.x = halo_width;
    size.y = zSize.y;
    size.z = zSize.z;

    drawAregion(start, size, shift);

    glPopMatrix();
}

void MPIProcessor::draw_without_halo()
{
    glPushMatrix();

    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v((ColorName)myrank);
    cube->set_cubecolor(c4v);
  //cube->set_alpha(1.0);

    glScalef(0.75, 0.75, 0.75);

    shift.x = 2.0 * (zRank.col - zPlane.col/2) * delta;
    shift.y = 2.0 * (zRank.row - zPlane.row/2) * delta;
    shift.z = 0.0;
    drawAregion(zStart, zSize, shift);

    glPopMatrix();
}

void MPIProcessor::draw_decomposed()
{
    glPushMatrix();

    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v((ColorName)myrank);
    cube->set_cubecolor(c4v);

    shift.x = 0.0;
    shift.y = 0.0;
    shift.z = 0.0;
    drawAregion(zStart, zSize, shift);

    glPopMatrix();
}

void MPIProcessor::draw_global_domain()
{
    pointstruct sp = {0, 0, 0};

    glPushMatrix();

    c4v = mpi_color->get_color4v(COLOR_BLUE);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v(COLOR_WHEAT);
    cube->set_cubecolor(c4v);

    shift.x = 0.0;
    shift.y = 0.0;
    shift.z = 0.0;
    drawAregion(sp, global_size, shift);

    glPopMatrix();
}

void MPIProcessor::drawAregion(pointstruct start, pointstruct region, shiftstruct shift)
{
    int i, j, k;
    float x, y, z;

  //glPushMatrix();

    for(j = 0; j < region.y; ++j)
    {
        y = (j + start.y - jc) * delta + shift.y;
        for(i = 0; i < region.x; ++i)
        {
            x = (i + start.x - ic) * delta + shift.x;
            for(k = 0; k < region.z; ++k)
            {
                z = (k + start.z - kc) * delta + shift.z;
                cube->set_origin(x, y, z);
                cube->draw();
            }
        }
    }

  //glPopMatrix();
}

void MPIProcessor::set_global_size(int nx, int ny, int nz)
{
    global_size.x = nx;
    global_size.y = ny;
    global_size.z = nz;
    zSize = global_size;

    ic = (1 + global_size.x) / 2;
    jc = (1 + global_size.y) / 2;
    kc = (1 + global_size.z) / 2;

    scale = 1.0 / global_size.x;
    delta = 2.0 * scale;

    cube->set_length(delta);
    cube->set_alpha(1.0);
}

void MPIProcessor::decomposition(int col, int row)
{
    zPlane.col = col;
    zPlane.row = row;
    zPlane.lev = 1;

    zSize.x = global_size.x / zPlane.col;
    zSize.y = global_size.y / zPlane.row;
    zSize.z = global_size.z / zPlane.lev;

    yPlane.col = col;
    yPlane.row = 1;
    yPlane.lev = row;

    ySize.x = global_size.x / yPlane.col;
    ySize.y = global_size.y / yPlane.row;
    ySize.z = global_size.z / yPlane.lev;

    xPlane.col = 1;
    xPlane.row = row;
    xPlane.lev = col;

    xSize.x = global_size.x / xPlane.col;
    xSize.y = global_size.y / xPlane.row;
    xSize.z = global_size.z / xPlane.lev;
}

void MPIProcessor::set_rank(int n, int nc, int nr)
{
    myrank = n;
    zRank.col = nc;
    zRank.row = nr;
    zRank.lev = 1;

    zStart.x = nc * zSize.x;
    zStart.y = nr * zSize.y;
    zStart.z = 0;

    yRank.col = nc;
    yRank.row = 0;
    yRank.lev = nr;

    yStart.x = nc * ySize.x;
    yStart.y = 0;
    yStart.z = nr * ySize.z;

    xRank.col = 0;
    xRank.row = nr;
    xRank.lev = nc;

    xStart.x = 0;
    xStart.y = nr * xSize.y;
    xStart.z = nc * xSize.z;
}

void MPIProcessor::xfactor(int n, int base)
{
    float s = 0.5 * (n + base) / (float) base;
    xc4v[0] = s * c4v[0];
    xc4v[1] = s * c4v[1];
    xc4v[2] = s * c4v[2];
    xc4v[3] = c4v[3];
}

void MPIProcessor::yfactor(int n, int base)
{
    float s = 0.5 * (2.0*base - n) / (float) base;
    yc4v[0] = s * c4v[0];
    yc4v[1] = s * c4v[1];
    yc4v[2] = s * c4v[2];
    yc4v[3] = c4v[3];
}

void MPIProcessor::draw_xtranspose_whole()
{
    glPushMatrix();

    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v((ColorName)myrank);
    cube->set_cubecolor(c4v);
  //cube->set_alpha(1.0);

    glScalef(0.75, 0.75, 0.75);

    shift.x = 0.0;
    shift.y = 4.0 * (zRank.row - zPlane.row/2) * delta;
    shift.z = 0.0;
    drawAregion(zStart, zSize, shift);

    glPopMatrix();
}

void MPIProcessor::draw_xtransposed()
{
    int k, n;
    pointstruct start, size;
    float s = 0.50;

    glPushMatrix();

    glScalef(s, s, s);

    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);

    shift.x = 0.0;
    shift.y = 6.0 * (xRank.row - xPlane.row/2) * delta;
    shift.z = 4.0 * (xRank.lev - xPlane.lev/2) * delta;

  //Each x-strip should have zPlane.col colors.
  //We also wants each level has different color as well.

    start.x = xStart.x;
    start.y = xStart.y;
    start.z = xStart.z;

    size.x = xSize.x / xPlane.lev;
    size.y = xSize.y;
    size.z = xSize.z;

  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\txRank.row = " << xRank.row << ", xPlane.row = " << xPlane.row << endl;
  //cout << "\txRank.lev = " << xRank.lev << ", xPlane.lev = " << xPlane.lev << endl;
  //cout << "\tmyrank = " << myrank << endl;

    for(k = 0; k < xPlane.lev; ++k)
    {
        n = xRank.row * xPlane.lev + k;
        c4v = mpi_color->get_color4v((ColorName)n);
        xfactor(xRank.lev, xPlane.lev);
        cube->set_cubecolor(xc4v);

        start.x = k * zSize.x;
        drawAregion(start, size, shift);
    }

    glPopMatrix();
}

void MPIProcessor::draw_ytranspose_whole()
{
    float s = 0.75;
    glPushMatrix();

    glScalef(s, s, s);

    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);
    c4v = mpi_color->get_color4v((ColorName)myrank);
    cube->set_cubecolor(c4v);
  //cube->set_alpha(1.0);

    shift.x = 4.0 * (zRank.col - zPlane.col/2) * delta;
    shift.y = 0.0;
    shift.z = 0.0;
    drawAregion(zStart, zSize, shift);

    glPopMatrix();
}

void MPIProcessor::draw_ytransposed()
{
    int k, n;
    pointstruct start, size;
    float s = 0.50;

    glPushMatrix();

    glScalef(s, s, s);

    c4v = mpi_color->get_color4v(COLOR_GRAY);
    cube->set_linecolor(c4v);

    shift.x = 6.0 * (zRank.col - zPlane.col/2) * delta;
    shift.y = 0.0;
    shift.z = 4.0 * (yRank.lev - yPlane.lev/2) * delta;

  //Each y-strip should have zPlane.row colors.
  //We also wants each level has different color as well.

    start.x = yStart.x;
    start.y = yStart.y;
    start.z = yStart.z;

    size.x = ySize.x;
    size.y = ySize.y / yPlane.lev;
    size.z = ySize.z;

  //cout << "\nfile: <" << __FILE__ << ">";
  //cout << "\tFunctions: <" << __PRETTY_FUNCTION__ << ">";
  //cout << "\tline: " << __LINE__ << endl;
  //cout << "\tyRank.col = " << yRank.col << ", yPlane.col = " << yPlane.col << endl;
  //cout << "\tyRank.lev = " << yRank.lev << ", yPlane.lev = " << yPlane.lev << endl;
  //cout << "\tmyrank = " << myrank << endl;

    for(k = 0; k < yPlane.lev; ++k)
    {
        n = yRank.col + yPlane.col * k;
        c4v = mpi_color->get_color4v((ColorName)n);
        yfactor(yRank.lev, yPlane.lev);
        cube->set_cubecolor(yc4v);

        start.y = k * size.y;
        drawAregion(start, size, shift);
    }

    glPopMatrix();
}

