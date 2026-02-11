//$Id: texturekeeper.cpp 4865 2014-01-01 15:49:23Z starviewer $

#include <QtOpenGL>

#include "texturekeeper.h"

TextureKeeper::TextureKeeper()
{
    _nx = 0;
    _nz = 0;
    _ny = 0;

    _xid = NULL;
    _yid = NULL;
    _zid = NULL;

    setup(801, 801, 121);
}

TextureKeeper::~TextureKeeper()
{
    if(NULL != _xid)
        free(_xid);
    if(NULL != _yid)
        free(_yid);
    if(NULL != _zid)
        free(_zid);
}

void TextureKeeper::set_xid(int n, unsigned int id)
{
    if((n >= _nx) && (n < 0))
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tn = " << n << " is greater then _nx = " << _nx << endl;
        return;
    }

    if(NULL == _xid)
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\t_xid is NULL." << endl;
        return;
    }

    _xid[n] = id;
}

void TextureKeeper::set_yid(int n, unsigned int id)
{
    if((n >= _ny) && (n < 0))
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tn = " << n << " is greater then _ny = " << _ny << endl;
        return;
    }

    if(NULL == _yid)
    {   
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\t_yid is NULL." << endl;
        return;
    }

    _yid[n] = id;
}

void TextureKeeper::set_zid(int n, unsigned int id)
{
    if((n >= _nz) && (n < 0))
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tn = " << n << " is greater then _nz = " << _nz << endl;
        return;
    }

    if(NULL == _zid)
    {   
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\t_zid is NULL." << endl;
        return;
    }

  //cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
  //     << ", file: <" << __FILE__ << ">" << endl;
  //cout << "\tsave _zid[" << n << "] as " << id << endl;

    _zid[n] = id;
}

unsigned int TextureKeeper::get_xid(int n)
{
    if((n >= _nx) && (n < 0))
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tn = " << n << " is greater then _nx = " << _nx << endl;
        return 0;
    }

    return _xid[n];
}

unsigned int TextureKeeper::get_yid(int n)
{
    if((n >= _ny) && (n < 0))
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tn = " << n << " is greater then _ny = " << _ny << endl;
        return 0;
    }

    return _yid[n];
}

unsigned int TextureKeeper::get_zid(int n)
{
    if((n >= _nz) && (n < 0))
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tn = " << n << " is greater then _nz = " << _nz << endl;
        return 0;
    }

    return _zid[n];
}

void TextureKeeper::setup(int nx, int ny, int nz)
{
    if((nx <= 0) || (ny <= 0) || (nz <= 0))
    {
        cout << "Functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__
             << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tnx = " << nx << ", ny = " << ny << ", nz = " << nz
             << " are un-proper to malloc" << endl;
        return;
    }

    _nx = nx;
    _ny = ny;
    _nz = nz;

    if(NULL == _xid)
    {
        _xid = (unsigned int*)calloc(_nx, sizeof(unsigned int));
        assert(_xid);
    }

    if(NULL == _yid)
    {
        _yid = (unsigned int*)calloc(_ny, sizeof(unsigned int));
        assert(_yid);
    }

    if(NULL == _zid)
    {
        _zid = (unsigned int*)calloc(_nz, sizeof(unsigned int));
        assert(_zid);
    }
}

void TextureKeeper::reset()
{
    reinitialize(_nx, _ny, _nz);
}

void TextureKeeper::reinitialize(int nx, int ny, int nz)
{
    int n;

    for(n = 0; n < _nx; ++n)
    {
        if(0 < _xid[n])
        {
            if(glIsTexture(_xid[n]))
                glDeleteTextures(1, &_xid[n]);
        }

        _xid[n] = 0;
    }

    for(n = 0; n < _ny; ++n)
    {
        if(0 < _yid[n])
        {
            if(glIsTexture(_yid[n]))
                glDeleteTextures(1, &_yid[n]);
        }

        _yid[n] = 0;
    }

    for(n = 0; n < _nz; ++n)
    {
        if(0 < _zid[n])
        {
            if(glIsTexture(_zid[n]))
                glDeleteTextures(1, &_zid[n]);
        }

        _zid[n] = 0;
    }

    if(nx > _nx)
    {
        _xid = (unsigned int*)realloc(_xid, nx * sizeof(unsigned int));
        assert(_xid);

        for(n = _nx; n < nx; ++n)
            _xid[n] = 0;
   
        _nx = nx;
    }

    if(ny > _ny)
    {
        _yid = (unsigned int*)realloc(_yid, ny * sizeof(unsigned int));
        assert(_yid);

        for(n = _ny; n < ny; ++n)
            _yid[n] = 0;
   
        _ny = ny;
    }

    if(nz > _nz)
    {
        _zid = (unsigned int*)realloc(_zid, nz * sizeof(unsigned int));
        assert(_zid);

        for(n = _nz; n < nz; ++n)
            _zid[n] = 0;
   
        _nz = nz;
    }
}

