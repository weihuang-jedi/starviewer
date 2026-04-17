#include <QOpenGLWidget>
#include "colorTable.h"

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

//Constructor
ColorTable::ColorTable()
{
    _cmap = NULL;
    _opacity = NULL;
    texture1d = NULL;

  //cout << "\tEnter ColorTable: file: " << __FILE__ << ", line: " << __LINE__ << endl;

    _setup();

  //print();

  //cout << "\tLeave ColorTable: file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

ColorTable::~ColorTable()
{
  //cout << "\tEnter ~ColorTable: file: " << __FILE__ << ", line: " << __LINE__ << endl;

  //if(NULL != texture1d)
  //    delete texture1d;

    if(NULL != _cmap)
        free(_cmap);
    if(NULL != _opacity)
        free(_opacity);

  //cout << "\tLeave ~ColorTable: file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void ColorTable::print()
{
    int n;
    int m;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    if(NULL != _cmap)
    {
        m = 0;
        for(n = 0; n < _clen; ++n)
        {
          //cout << "Color " << n << ": (" << _cmap[m] << ", "
          //                               << _cmap[m+1] << ", "
          //                               << _cmap[m+2] << ")" << endl;
            m += 3;
        }
    }
}

void ColorTable::_setup()
{
    int num = 0;
    DIR *dp;
    struct dirent *dirp;
    char root[1024];

    char *cp;
    int dot_pos;

  //cout << "\tEnter _setup: file: " << __FILE__ << ", line: " << __LINE__ << endl;

    string curdir = ".";
    string pardir = "..";

  //cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(root, path);
  //strcpy(root, "/contrib/Wei.Huang/src/nv/starviewer");
  //cout << "root: " << root << endl;
    strcat(root, "/colormaps/");
  //cout << "root: " << root << endl;

  //cout << "\troot: <" << root << ">" << endl;
  //cout << "\tUse default color map: gui_default" << endl;

    _opacity = NULL;
    _cmap = NULL;
    _clen = 0;

  //cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

  //set_colorMap(string("ncl_default"));
    set_colorMap(string("gui_default"));

  //cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\t_name: <" << _name << ">" << endl;

  //cout << "\tdir to get files of: <" << root << ">" << endl;

    if(NULL == (dp = opendir(root)))
    {
        cout << "Error(" << errno << ") opening " << root << endl;
        throw(errno);
    }

    while((dirp = readdir(dp)))
    {
        if((dirp->d_name == curdir) || (dirp->d_name == pardir))
            continue;

        if(! (cp = strrchr(dirp->d_name,'.')))
            continue;
        dot_pos = cp - dirp->d_name;
        cp++;
        if(! cp || (strcmp(cp,"rgb") && strcmp(cp,"ncmap") && strcmp(cp,"gp")))
            continue;

        cp = dirp->d_name;
        cp[dot_pos] = '\0';

      //cout << "File " << num << ": <" << cp << ">" << endl;
        ++num;

        _clist.push_back(cp);
    }

    closedir(dp);

  //cout << "\tLeave _setup: file: " << __FILE__ << ", line: " << __LINE__ << endl;
}

void ColorTable::get_file_contents(const char *cn)
{
    ifstream in;
    string contents = "BLANK";

    char root[1024];
    char fullname[1024];
    char cstr[1024];
    char *pstr;
    char carr[4][128];

    vector<string> strvector;

    int i = 0;
    int m = 0;
    int n = 0;
    int nl = 0;
    int guessed_clen = 0;
    unsigned int k = 0;

    float maxval = 1.0;

    const char* path = getenv("STARVIEWERHOME");
    if (path == nullptr) {
        cout << "ERROR: STARVIEWERHOME not set!" << endl;
        throw(errno);
    }
    strcpy(root, path);
    strcat(root, "/colormaps/");

    strcat(root, cn);

    strcpy(fullname, root);
    strcat(fullname, ".rgb");

    in.open(fullname, ios::in | ios::binary);

    if(! in.is_open())
    {
        in.close();
        strcpy(fullname, root);
        strcat(fullname, ".gp");
        in.open(fullname, ios::in | ios::binary);

        if(! in.is_open())
        {
            in.close();
            strcpy(fullname, root);
            strcat(fullname, ".ncmap");
            in.open(fullname, ios::in | ios::binary);

            if(! in.is_open())
            {
                in.close();
                throw(errno);
            }
        }
    }

  //cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\troot: <" << root << ">" << endl;
  //cout << "\tfullname: <" << fullname << ">" << endl;

  //in.seekg(0, ios::end);
  //contents.resize(in.tellg());
  //in.seekg(0, ios::beg);
  //in.read(&contents[0], contents.size());
  //in.close();

    nl = 0;
    while(getline(in, contents))
    {
        ++nl;
        strvector.push_back(contents);
    }

    in.close();

    guessed_clen = 3 * nl;

    if(NULL != _cmap)
        free(_cmap);
    if(NULL != _opacity)
        free(_opacity);

    nl = guessed_clen;
    _cmap = (float *) calloc(guessed_clen, sizeof(float));
    assert(_cmap);
    _opacity = (float *) calloc(nl, sizeof(float));
    assert(_opacity);

    _clen = 0;
    for(k = 0; k < strvector.size(); ++k)
    {
        strcpy(cstr, strvector[k].c_str());

      //cout << "Line " << k << ": <" << cstr << ">" << endl;

        n = 0;
        pstr = strtok(cstr, " \t");
        while(NULL != pstr)
        {
            strcpy(carr[n], pstr);

          //if(n)
          //    cout << ", carr[" << n << "] = <" << carr[n] << ">";
          //else
          //    cout << "\tcarr[" << n << "] = <" << carr[n] << ">";

            pstr = strtok(NULL, " \t");

            ++n;
        }

      //cout << endl;

        m = 0;
        for(i = 0; i < n; ++i)
        {
            if((! isdigit(carr[i][0])) && ('.' != carr[i][0]))
            {
                break;
            }
            ++m;
        }

        for(i = 0; i < m; ++i)
        {
            contents = string(carr[i]);

          //if(i)
          //    cout << ", carr[" << i << "] = <" << carr[i] << ">" << endl;
          //else
          //    cout << "\tcarr[" << i << "] = <" << carr[i] << ">" << endl;

            istringstream(contents) >> _cmap[_clen];

            if(maxval < _cmap[_clen])
                maxval = _cmap[_clen];

          //cout << "_cmap[" << _clen << "] = " << _cmap[_clen] << endl;

            ++ _clen;
        }
    }

    if(nl != _clen)
    {
        _cmap = (float *) realloc(_cmap, _clen * sizeof(float));
        assert(_cmap);
        _opacity = (float *) realloc(_opacity, (_clen/3) * sizeof(float));
        assert(_opacity);
    }

    if(1.0 < maxval)
    {
        if(256.0 > maxval)
        {
            for(n = 0; n < _clen; ++n)
            {
                _cmap[n] /= 255.0;
            }
        }
        else if(256.0 == maxval)
        {
            for(n = 0; n < _clen; ++n)
            {
                _cmap[n] /= 256.0;
            }
        }
        else if(65536.0 > maxval)
        {
            for(n = 0; n < _clen; ++n)
            {
                _cmap[n] /= 65535.0;
            }
        }
        else
        {
            for(n = 0; n < _clen; ++n)
            {
                _cmap[n] /= 65536.0;
            }
        }
    }

    _clen /= 3;

    for(n = 0; n < _clen; ++n)
    {
        _opacity[n] = 0.10 + 0.9*n / ((float) _clen);
    }

  //print();

#if 0
  //Switch background and foreground color.
    if(1 < _clen)
    {
        for(n = 0; n < 3; ++n)
        {
            maxval = _cmap[n];
            _cmap[n] = _cmap[n+3];
            _cmap[n+3] = maxval;
        }
    }
#endif
}

void ColorTable::set_colorMap(string cn)
{
    _name = cn;

  //cout << "\tfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\t_name: <" << _name << ">" << endl;

    get_file_contents(cn.c_str());

    if(NULL != texture1d)
        delete texture1d;

    texture1d = new Texture1d();
    texture1d->set_colors(_clen, _cmap);
    texture1d->set_name(cn);
}

void ColorTable::set_opacity(int opt)
{
    int n;
    switch(opt)
    {
        case -1:
             for(n = 0; n < _clen; ++n)
             {
                 _opacity[n] = 0.10 + 0.9* (_clen - n) / ((float) _clen);
             }
             break;
        case 0:
             for(n = 0; n < _clen; ++n)
             {
                 _opacity[n] = 0.10 + 0.45 * abs(_clen - 2*n)/ ((float) _clen);
             }
             break;
        case 1:
             for(n = 0; n < _clen; ++n)
             {
                 _opacity[n] = 0.10 + 0.9*n / ((float) _clen);
             }
             break;
        default:
             for(n = 0; n < _clen; ++n)
             {
                 _opacity[n] = 0.50;
             }
             break;
    }
}

void ColorTable::set_color4v(double v, double* color4v)
{
    int i, n;
    double d;

    d = v * _clen;
    n = (int) d;
    d -= (double) n;

    if(n)
    {
        for(i = 0; i < 3; ++i)
            color4v[i] = (1.0-d) * _cmap[3*n + i] + d  * _cmap[3*(n + 1) + i];
        color4v[3] = _opacity[n];
    }
    else
    {
        for(i = 0; i < 3; ++i)
            color4v[i] = 1.0;
        color4v[3] = 0.0;
    }
}
