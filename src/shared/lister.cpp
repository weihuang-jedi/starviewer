//$Id: lister.cpp 4833 2013-11-05 15:49:31Z starviewer $

#include <QOpenGLWidget>

#include "lister.h"

using namespace std;

Lister::Lister()
{
    _nx = 0;
    _nz = 0;
    _ny = 0;

    _xid = NULL;
    _yid = NULL;
    _zid = NULL;

    setup(801, 801, 121);
}

Lister::~Lister()
{
    if(NULL != _xid)
        free(_xid);
    if(NULL != _yid)
        free(_yid);
    if(NULL != _zid)
        free(_zid);
}

void Lister::set_xid(int n, GLuint id)
{
    if((n >= _nx) && (n < 0))
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\tn = " << n << " is greater then _nx = " << _nx << endl;
#endif
        return;
    }

    if(NULL == _xid)
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\t_xid is NULL." << endl;
#endif
        return;
    }

    _xid[n] = id;
}

void Lister::set_yid(int n, GLuint id)
{
    if((n >= _ny) && (n < 0))
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\tn = " << n << " is greater then _ny = " << _ny << endl;
#endif
        return;
    }

    if(NULL == _yid)
    {   
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\t_yid is NULL." << endl;
#endif
        return;
    }

    _yid[n] = id;
}

void Lister::set_zid(int n, GLuint id)
{
    if((n >= _nz) && (n < 0))
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\tn = " << n << " is greater then _nz = " << _nz << endl;
#endif
        return;
    }

    if(NULL == _zid)
    {   
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\t_zid is NULL." << endl;
#endif
        return;
    }

#if 0
  //std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //std::cout << "\tsave _zid[" << n << "] as " << id << endl;
#endif

    _zid[n] = id;
}

GLuint Lister::get_xid(int n)
{
    if((n >= _nx) && (n < 0))
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\tn = " << n << " is greater then _nx = " << _nx << endl;
#endif
        return 0;
    }

    return _xid[n];
}

GLuint Lister::get_yid(int n)
{
    if((n >= _ny) && (n < 0))
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\tn = " << n << " is greater then _ny = " << _ny << endl;
#endif
        return 0;
    }

    return _yid[n];
}

GLuint Lister::get_zid(int n)
{
    if((n >= _nz) && (n < 0))
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\tn = " << n << " is greater then _nz = " << _nz << endl;
#endif
        return 0;
    }

    return _zid[n];
}

void Lister::setup(int nx, int ny, int nz)
{
    if((nx <= 0) || (ny <= 0) || (nz <= 0))
    {
#if 0
        std::cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        std::cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz
             << " are un-proper to malloc" << endl;
#endif
        return;
    }

    _nx = nx;
    _ny = ny;
    _nz = nz;

    if(NULL == _xid)
    {
        _xid = (GLuint*)calloc(_nx, sizeof(GLuint));
        assert(_xid);
    }

    if(NULL == _yid)
    {
        _yid = (GLuint*)calloc(_ny, sizeof(GLuint));
        assert(_yid);
    }

    if(NULL == _zid)
    {
        _zid = (GLuint*)calloc(_nz, sizeof(GLuint));
        assert(_zid);
    }
}

void Lister::reset()
{
    reinitialize(_nx, _ny, _nz);
}

void Lister::reinitialize(int nx, int ny, int nz)
{
    int n;

    for(n = 0; n < _nx; ++n)
    {
        if(0 < _xid[n])
        {
            if(glIsList(_xid[n]))
                glDeleteLists(_xid[n], 1);
        }

        _xid[n] = 0;
    }

    for(n = 0; n < _ny; ++n)
    {
        if(0 < _yid[n])
        {
            if(glIsList(_yid[n]))
                glDeleteLists(_yid[n], 1);
        }

        _yid[n] = 0;
    }

    for(n = 0; n < _nz; ++n)
    {
        if(0 < _zid[n])
        {
            if(glIsList(_zid[n]))
                glDeleteLists(_zid[n], 1);
        }

        _zid[n] = 0;
    }

    if(nx > _nx)
    {
        _xid = (GLuint*)realloc(_xid, nx * sizeof(GLuint));
        assert(_xid);

        for(n = _nx; n < nx; ++n)
            _xid[n] = 0;
   
        _nx = nx;
    }

    if(ny > _ny)
    {
        _yid = (GLuint*)realloc(_yid, ny * sizeof(GLuint));
        assert(_yid);

        for(n = _ny; n < ny; ++n)
            _yid[n] = 0;
   
        _ny = ny;
    }

    if(nz > _nz)
    {
        _zid = (GLuint*)realloc(_zid, nz * sizeof(GLuint));
        assert(_zid);

        for(n = _nz; n < nz; ++n)
            _zid[n] = 0;
   
        _nz = nz;
    }
}

