#include "nclfilehandler.h"

template<typename T>
T string2number(string sn)
{
    T value;

    stringstream stream(sn);
    stream >> value;
    if(stream.fail())
    {
        cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
        cout << "\tFailed convert " << sn << " to T." << endl;
        exit (-1);
    }

    return value;
}

NclFileHandler::NclFileHandler()
{
    _default();
}

NclFileHandler::~NclFileHandler()
{
    delete [] _ndv;

    if(NULL != _atts)
        delete [] _atts;
    if(NULL != _dims)
       delete [] _dims;
    if(NULL != _vars)
        delete [] _vars;
    if(NULL != _coordvars)
        delete [] _coordvars;
    if(NULL != _nclvar)
        free(_nclvar);
}

void NclFileHandler::_default()
{
    _natts = 0;
    _ndims = 0;
    _nvars = 0;
    _ncoords = 0;

    _nclvar = NULL;

    _atts = NULL;
    _dims = NULL;
    _vars = NULL;
    _coordvars = NULL;

    _hasTime = false;

    _ndv = new int[MAXDIM];

    memset(_ndv, 0, MAXDIM * sizeof(int));

    _hascoords = false;
    _oldcoordstring = "";
}

void NclFileHandler::open(string fn)
{
    _flnm = fn;

    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
    cout << "\tfilename: <" << _flnm << ">" << endl;
    cout << "\tfilename: <" << _flnm.c_str() << ">" << endl;
 
    _nclfile = NclCreateFile(_flnm.c_str());

    _check_atts();
    _check_dims();
    _check_vars();
}

void NclFileHandler::_check_dims()
{
    int n;
    NclAdvancedFile theadvancedfile = NULL;
    NclFileGrpNode *grpnode = NULL;

    char* dimname;
    int dimsize;

    Dimension* dim;
 
    _ndims = 0;
  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

    if(_nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
        theadvancedfile = (NclAdvancedFile) _nclfile;
        grpnode = theadvancedfile->advancedfile.grpnode;
        if(NULL != grpnode->dim_rec)
            _ndims = grpnode->dim_rec->n_dims;
    }
    else
    {
        _ndims = _nclfile->file.n_file_dims;
    }

    if(1 > _ndims)
        return;

    _dims = new Dimension[_ndims];

  //cout << "\tndims = " << _ndims << "." << endl;
  
    if(_nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
        for(n = 0; n < _ndims; ++n)
        {
            dimname = guiQuarkToString(grpnode->dim_rec->dim_node[n].name);
            dimsize = (int) grpnode->dim_rec->dim_node[n].size;

            dim = &(_dims[n]);
            dim->set_name(dimname);
            dim->set_size(dimsize);
        }
    }
    else
    {
      //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        for(n = 0; n < _ndims; ++n)
        {
            dimname = guiQuarkToString(_nclfile->file.file_dim_info[n]->dim_name_quark);
            dimsize = (int) _nclfile->file.file_dim_info[n]->dim_size;

            dim = &(_dims[n]);
            dim->set_name(dimname);
            dim->set_size(dimsize);

            if(0 == strcmp(dimname, "time"))
                _hasTime = true;
            else if(0 == strcmp(dimname, "Time"))
                _hasTime = true;

          //cout << "\tDim " << n << ": name <" << dim->get_name() << ">, size = " << dim->get_size() << endl;
        }
    }
}

void NclFileHandler::_check_atts()
{
    int n;
    char **attnames = NULL;

    int   *iptr;
    char  *cptr;
    float *fptr;
    double *dptr;

    NclMultiDValData attMV = NULL;

    Attribute* att = NULL;
 
    _natts = 0;

    attnames = guiGetNclFileAttNames(_nclfile, &_natts);

    if(1 > _natts)
        return;

    _atts = new Attribute[_natts];

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << ">: _natts = " << _natts << "." << endl;
    for(n = 0; n < _natts; ++n)
    {
      //cout << "\tAtt " << n << ": <" << attnames[n] << ">" << endl;
        attMV = guiGetFileAtt(_nclfile, attnames[n]);

        att = &(_atts[n]);
        att->set_name(attnames[n]);

        if(NCL_string == attMV->multidval.data_type)
        {
            cptr = guiQuarkToString(*(NclQuark *)attMV->multidval.val);
            att->set_value(cptr);
        }
        else if(NCL_int == attMV->multidval.data_type)
        {
            iptr = (int *)attMV->multidval.val;
            att->set_value(iptr[0]);
        }
        else if(NCL_float == attMV->multidval.data_type)
        {
            fptr = (float *)attMV->multidval.val;
            att->set_value(fptr[0]);
        }
        else if(NCL_double == attMV->multidval.data_type)
        {
            dptr = (double *)attMV->multidval.val;
            att->set_value(dptr[0]);
        }
        else
            att->set_value("do not care");

      //cout << "\tAtt " << n << ": <" << att->get_name() << "> = <" << att->get_value() << ">" << endl;

      //guiDestroyObj((NclObj) attMV);
    }
}

void NclFileHandler::_check_vars()
{
    int n;
    int i;
    int j;

    NclAdvancedFile theadvancedfile = NULL;
    NclFileGrpNode *grpnode = NULL;
    NclFileVarNode *varnode = NULL;
    NclFileDimNode *dimnode = NULL;

    Variable* var;
  //Dimension* dim;
  //Attribute* att = NULL;

    NclMultiDValData attMV = NULL;
    NclFileAttInfoList* step;

    string varname;
    string dimname;
    string attname;
    string cptr;
    string coordnames;
    string coordstring;

    double *dptr;
    float *fptr;
    float  fval;
    int   *iptr;
    short *sptr;
    int    dimsize;
    int    nvds;

    _nvars = 0;
    if(_nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
        theadvancedfile = (NclAdvancedFile) _nclfile;
        grpnode = theadvancedfile->advancedfile.grpnode;
        if(NULL != grpnode->var_rec)
            _nvars = grpnode->var_rec->n_vars;
    }
    else
    {
        _nvars = _nclfile->file.n_vars;
    }
 
  //cout << "\nfile: <" << __FILE__ << ">, function: <"
  //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tnvars = " << _nvars << "." << endl;

    if(0 == _nvars)
        return;

    _vars = new Variable[_nvars];

  //_coordvars are hard-coded here for now. 8/1/2013
    _coordvars = new Variable[MAXCOORDS];

    if(_nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
      //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        for(n = 0; n < _nvars; ++n)
        {
            varnode = &(grpnode->var_rec->var_node[n]);
            varname = guiQuarkToString(varnode->name);

            var = &(_vars[n]);
            var->set_name(string(varname));

          //cout << "\tVar " << n << ": <" << var->get_name() << ">" << endl;

            if(NULL != varnode->dim_rec)
            {
                nvds = varnode->dim_rec->n_dims;
                var->set_ndims(nvds);
                ++_ndv[nvds];

                for(i = 0; i < varnode->dim_rec->n_dims; ++i)
                {
                    dimnode = &(varnode->dim_rec->dim_node[n]);
                    dimsize = (int)dimnode->size;
                    dimname = guiQuarkToString(dimnode->name);
                    var->push_dim(dimname, dimsize);

                  //cout << "\t\tDim " << i << ": name <" << dimname << ">, size = " << dimsize << endl;
                }
            }
        }
    }
    else
    {
      //cout << "\nfile: <" << __FILE__ << ">, function: <"
      //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

        for(n = 0; n < _nvars; ++n)
        {
            varname = guiQuarkToString(_nclfile->file.var_info[n]->var_name_quark);
            var = &(_vars[n]);
            var->set_name(varname);
            var->set_hasFillValue(false);

            nvds = (int)_nclfile->file.var_info[n]->num_dimensions;
            var->set_ndims(nvds);
            ++_ndv[nvds];

          //cout << "\nfile: <" << __FILE__ << ">, function: <"
          //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
          //cout << "\tVar " << n << ": <" << var->get_name() << ">" <<
          //        ", number of dims " << nvds <<
          //        ", _ndv[ " << nvds << "] = " << _ndv[nvds] << endl;

            for(i = 0; i < nvds; ++i)
            {
                j = _nclfile->file.var_info[n]->file_dim_num[i];

                dimsize = (int)_nclfile->file.file_dim_info[j]->dim_size;
                dimname = guiQuarkToString(_nclfile->file.file_dim_info[j]->dim_name_quark);

                var->push_dim(dimname, dimsize);

              //cout << "\t\tDim " << i << ": name <" << dimname << ">, size = " << dimsize << endl;
            }

          //cout << "\nfile: <" << __FILE__ << ">, function: <"
          //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;

            if(NULL != _nclfile->file.var_att_info[n])
            {
                j = 0;
                step = _nclfile->file.var_att_info[n];
                while(NULL != step)
                {
                    step = step->next;
                    ++j;
                }

                var->set_natts(j);

                step = _nclfile->file.var_att_info[n];
                j = 0;
                while(NULL != step)
                {
                    attname = guiQuarkToString(step->the_att->att_name_quark);
                    attMV = guiGetVarAttMV(_nclfile, varname.c_str(), attname.c_str());

                  //cout << "\nfile: <" << __FILE__ << ">, function: <"
                  //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
                  //cout << "\tAtt " << j << ": <" << attname << ">" << endl;
                  //cout << "\ttype: " << attMV->multidval.data_type
                  //     << ", NCL_string = " << NCL_string
                  //     << ", NCL_double = " << NCL_double
                  //     << ", NCL_float = " << NCL_float
                  //     << ", NCL_short = " << NCL_short
                  //     << ", NCL_int = " << NCL_int << endl;

                    if(0 == attname.compare("_FillValue"))
                    {
                        fval = 1.0e20;
                        switch(attMV->multidval.data_type)
                        {
                            case NCL_string:
                                cptr = guiQuarkToString(*(NclQuark *)attMV->multidval.val);
                                var->push_att(attname, cptr);
                                fval = string2number<float>(cptr);
                                var->set_hasFillValue(true);
                                var->set_fillValue(fval);
                                break;
                            case NCL_double:
                                dptr = (double *)attMV->multidval.val;
                                fval = (float) dptr[0];
                                var->push_att(attname, dptr[0]);
                                var->set_hasFillValue(true);
                                var->set_fillValue(fval);
                                break;
                            case NCL_float:
                                fptr = (float *)attMV->multidval.val;
                                fval = fptr[0];
                                var->push_att(attname, fptr[0]);
                                var->set_hasFillValue(true);
                                var->set_fillValue(fval);
                                break;
                            case NCL_int:
                                iptr = (int *)attMV->multidval.val;
                                fval = (float) iptr[0];
                                var->push_att(attname, iptr[0]);
                                var->set_hasFillValue(true);
                                var->set_fillValue(fval);
                                break;
                            case NCL_short:
                                sptr = (short *)attMV->multidval.val;
                                fval = (float) sptr[0];
                                var->push_att(attname, sptr[0]);
                                var->set_hasFillValue(true);
                                var->set_fillValue(fval);
                                break;
                            default:
                                break;
                        }

                      //cout << "\nfile: <" << __FILE__ << ">, function: <"
                      //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
                      //cout << "\tVar " << n << ": <" << var->get_name() << ">" << endl;
                      //cout << "\tAtt " << j << ": <" << attname << "> = " << fval << endl;
                    }
                    else if(NCL_string == attMV->multidval.data_type)
                    {
                        cptr = guiQuarkToString(*(NclQuark *)attMV->multidval.val);
                        var->push_att(attname, cptr);
                        coordnames = guiQuarkToString(step->the_att->att_name_quark);
                        if(0 == coordnames.compare("coordinates"))
                        {
                            _hascoords = true;
                            coordstring = cptr;

                          //cout << "\nfile: <" << __FILE__ << ">, function: <"
                          //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
                          //cout << "\tAtt " << j << ": <" << attname << "> = <" << cptr << ">" << endl;
                          //cout << "\tcoordinates: <" << coordstring << ">" << endl;
                            if(0 <= coordstring.compare(_oldcoordstring))
                                _check_coordvars(coordstring);
                        }
                    }
                    else if(NCL_short == attMV->multidval.data_type)
                    {
                        short *sptr = (short *)attMV->multidval.val;
                        var->push_att(attname, sptr[0]);
                      //cout << "\tAtt " << j << ": <" << attname << "> = <" << fptr[0] << ">" << endl;
                    }
                    else if(NCL_int == attMV->multidval.data_type)
                    {
                        iptr = (int *)attMV->multidval.val;
                        var->push_att(attname, iptr[0]);
                      //cout << "\tAtt " << j << ": <" << attname << "> = <" << iptr[0] << ">" << endl;
                    }
                    else if(NCL_float == attMV->multidval.data_type)
                    {
                        fptr = (float *)attMV->multidval.val;
                        var->push_att(attname, fptr[0]);
                      //cout << "\tAtt " << j << ": <" << attname << "> = <" << fptr[0] << ">" << endl;
                    }
                    else if(NCL_double == attMV->multidval.data_type)
                    {
                        dptr = (double *)attMV->multidval.val;
                        fval = (float) dptr[0];
                        var->push_att(attname, fval);
                      //cout << "\tAtt " << j << ": <" << attname << "> = <" << fval << ">" << endl;
                    }
                    else
                        var->push_att(attname, "do not care");

                    ++j;
                    step = step->next;
                }
            }
        }
    }
}

string NclFileHandler::getAndvName(int n)
{
    string vn = string("unknown");

    int i, nd;
    Variable* var;

    if(_hasTime)
        nd = n + 1;
    else
        nd = n;

    for(i = 0; i < _nvars; ++i)
    {
        var = &(_vars[i]);
        if(var->get_ndims() == nd)
        {
            vn = var->get_name();
            break;
        }
    }

    return vn;
}

void NclFileHandler::print()
{
    int n;
 
    cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
    cout << "\tnum_vars = " << _nvars << endl;

    for(n = 0; n < _nvars; ++n)
    {
        cout << "\tVar " << n << ": <" << _vars[n].get_name() << endl;
    }
}

int NclFileHandler::get_ndv(int n)
{
    if(_hasTime)
        return _ndv[n+1];
    else
        return _ndv[n];
}

string *NclFileHandler::get_ndvNames(int n)
{
    int i, j, nd;
    int nv;
    string *names = NULL;
    Variable* var;

    if(_hasTime)
        nd = n + 1;
    else
        nd = n;

    nv = _ndv[nd];

    if(nv)
    {
        names = new string[nv];

        j = 0;
        for(i = 0; i < _nvars; ++i)
        {
            var = &(_vars[i]);
            if(var->get_ndims() == nd)
            {
                names[j] = var->get_name();
                ++j;
            }
        }
    }

    return names;
}

bool NclFileHandler::_setvar(string vn)
{
    int n;
    Variable* var;
    string varname;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\ttry to find var: <" << vn << ">" << endl;

    _idx = -1;

    for(n = 0; n < _nvars; ++n)
    {
        var = &(_vars[n]);
        varname = var->get_name();

        if(0 == vn.compare(varname))
        {
            _idx = n;
          //_nclvar = readNclFileVar(_nclfile, vn.c_str(), NULL);

            return true;
        }
    }

    return false;
}

float* NclFileHandler::get_fv(string vn)
{
    bool found = _setvar(vn);
    float *value;
    string dimname;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    if(found)
    {
        _nclvar = readNclFileVar(_nclfile, vn.c_str(), NULL);

        value = guiGetValue(_nclvar);

        guiDestroyObj((NclObj) _nclvar);

        return value;
    }
    else
    {
        cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        cout << "\tCan not find var : <" << vn << "> in file: <" << _flnm << ">" << endl;
        return NULL;
    }
}

int* NclFileHandler::get_iv(string vn)
{
    bool found = _setvar(vn);
    int* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    if(found)
    {
        _nclvar = readNclFileVar(_nclfile, vn.c_str(), NULL);

        value = guiGetIntArray(_nclvar);

        guiDestroyObj((NclObj) _nclvar);

        return value;
    }
    else
    {
        cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        cout << "\tCan not find var : <" << vn << "> in file: <" << _flnm << ">" << endl;
        return NULL;
    }
}

double* NclFileHandler::get_dv(string vn)
{
    bool found = _setvar(vn);
    double* value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    if(found)
    {
        _nclvar = readNclFileVar(_nclfile, vn.c_str(), NULL);

        value = guiGetDoubleArray(_nclvar);

        guiDestroyObj((NclObj) _nclvar);

        return value;
    }
    else
    {
        cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        cout << "\tCan not find var : <" << vn << "> in file: <" << _flnm << ">" << endl;
        return NULL;
    }
}

string NclFileHandler::get_sv(string vn)
{
    bool found = _setvar(vn);
    string value;

  //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tvar name: <" << vn << ">" << endl;

    if(found)
    {
        char* cval;

        _nclvar = readNclFileVar(_nclfile, vn.c_str(), NULL);

        cval = guiGetCharArray(_nclvar);

      //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
      //cout << "\tcval = <" << cval << ">" << endl;

        value = string(cval);

        free(cval);

        guiDestroyObj((NclObj) _nclvar);

        return value;
    }
    else
    {
        cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        cout << "\tCan not find var : <" << vn << "> in file: <" << _flnm << ">" << endl;
        return string("");
    }
}

string* NclFileHandler::_get_coordnames(string coordstring, int &nc)
{
    string* cvnames = new string[MAXCOORDS];
    char thestr[1024];
    char delimiter[4] = "; ,";
    char* token;
    nc = 0;

  //cout << "\nfile: <" << __FILE__ << ">, function: <"
  //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "coordstring: " << coordstring << endl;

    strcpy(thestr, coordstring.c_str());
    token = strtok(thestr, delimiter);
 
    while( token != NULL )
    {
       cvnames[nc] = token;
       token = strtok( NULL, delimiter );
       ++nc;
    }

  //for ( int i = 0; i < nc; ++i)
  //    cout << "cvnames[" << i << "] = <" << cvnames[i] << ">" << endl;

    return cvnames;
}

void NclFileHandler::_check_coordvars(string coordstring)
{
    int i, j, m, n;
    int nc = 0;
    int np = 0;

    NclFileGrpNode *grpnode = NULL;
    NclFileVarNode *varnode = NULL;
    NclFileDimNode *dimnode = NULL;

    Variable* var;
    Variable* coordvar;
  //Dimension* dim;
  //Attribute* att = NULL;

    NclMultiDValData attMV = NULL;
    NclFileAttInfoList* step;

    string varname;
    string dimname;
    string attname;
    string cptr;

    double *dptr;
    float *fptr;
    float  fval;
    int   *iptr;
    short *sptr;
    int    dimsize;
    int    nvds;

    string* coordnames = NULL;

    if(0 == _oldcoordstring.compare(coordstring))
        return;

  //cout << "\nfile: <" << __FILE__ << ">, function: <"
  //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
  //cout << "\tcoordinates: <" << coordstring << ">" << endl;

    _oldcoordstring = coordstring;

    coordnames = _get_coordnames(coordstring, nc);

    if(_nclfile->file.advanced_file_structure)
    {
      //Advanced file strucuture
      //cout << "\nfile: <" << __FILE__ << ">, function: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
        for(n = 0; n < _nvars; ++n)
        {
            varnode = &(grpnode->var_rec->var_node[n]);
            varname = guiQuarkToString(varnode->name);

            var = &(_vars[n]);
            var->set_name(string(varname));

          //cout << "\tVar " << n << ": <" << var->get_name() << ">" << endl;

            if(NULL != varnode->dim_rec)
            {
                nvds = varnode->dim_rec->n_dims;

                for(i = 0; i < varnode->dim_rec->n_dims; ++i)
                {
                    dimnode = &(varnode->dim_rec->dim_node[n]);
                    dimsize = (int)dimnode->size;
                    dimname = guiQuarkToString(dimnode->name);
                  //var->push_dim(dimname, dimsize);

                  //cout << "\t\tDim " << i << ": name <" << dimname << ">, size = " << dimsize << endl;
                }
            }
        }
    }
    else
    {
      //cout << "\nfile: <" << __FILE__ << ">, function: <"
      //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
      //cout << "\tnc = " << nc << endl;

        for(m = 0; m < nc; ++m)
        {
          //cout << "\tCheck coord " << m << ": name <" << coordnames[m] << ">" << endl;
            np = 0;
            for(n = 0; n < _ncoords; ++n)
            {
                coordvar = &(_coordvars[n]);
                varname = coordvar->get_name();
              //cout << "\t\tcompare coord " << n << ": name <" << varname << ">" << endl;
                if(0 == varname.compare(coordnames[m]))
                {
                    ++np;
                    break;
                }
            }

          //cout << "\nfile: <" << __FILE__ << ">, function: <"
          //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
          //cout << "\tnp = " << np << endl;

            if(np)
                continue;

            for(n = 0; n < _nvars; ++n)
            {
                var = &(_vars[n]);
                varname = guiQuarkToString(_nclfile->file.var_info[n]->var_name_quark);

                if(0 != varname.compare(coordnames[m]))
                    continue;

                coordvar = &(_coordvars[_ncoords]);
                ++_ncoords;

		if(MAXCOORDS <= _ncoords)
                {
                  //cout << "\nfile: <" << __FILE__ << ">, function: <"
                  //     << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << endl;
                  //cout << "\t_ncoords = " << _ncoords << " is greater than MAXCOORDS " << MAXCOORDS << endl;
                    exit (-1);
                }

                coordvar->set_name(varname);
                coordvar->set_hasFillValue(false);

                nvds = (int)_nclfile->file.var_info[n]->num_dimensions;
                coordvar->set_ndims(nvds);

                for(i = 0; i < nvds; ++i)
                {
                    j = _nclfile->file.var_info[n]->file_dim_num[i];

                    dimsize = (int)_nclfile->file.file_dim_info[j]->dim_size;
                    dimname = guiQuarkToString(_nclfile->file.file_dim_info[j]->dim_name_quark);

                    coordvar->push_dim(dimname, dimsize);
                }

                if(NULL != _nclfile->file.var_att_info[n])
                {
                    j = 0;
                    step = _nclfile->file.var_att_info[n];
                    while(NULL != step)
                    {
                        step = step->next;
                        ++j;
                    }

                    coordvar->set_natts(j);

                    step = _nclfile->file.var_att_info[n];
                    j = 0;
                    while(NULL != step)
                    {
                        attname = guiQuarkToString(step->the_att->att_name_quark);
                        attMV = guiGetVarAttMV(_nclfile, varname.c_str(), attname.c_str());

                        if(NCL_short == attMV->multidval.data_type)
                        {
                            sptr = (short *)attMV->multidval.val;
                            coordvar->push_att(attname, sptr[0]);
                          //cout << "\tAtt " << j << ": <" << attname << "> = <" << fptr[0] << ">" << endl;
                        }
                        else if(NCL_int == attMV->multidval.data_type)
                        {
                            iptr = (int *)attMV->multidval.val;
                            coordvar->push_att(attname, iptr[0]);
                          //cout << "\tAtt " << j << ": <" << attname << "> = <" << iptr[0] << ">" << endl;
                        }
                        else if(NCL_float == attMV->multidval.data_type)
                        {
                            fptr = (float *)attMV->multidval.val;
                            coordvar->push_att(attname, fptr[0]);
                          //cout << "\tAtt " << j << ": <" << attname << "> = <" << fptr[0] << ">" << endl;
                        }
                        else if(NCL_double == attMV->multidval.data_type)
                        {
                            dptr = (double *)attMV->multidval.val;
                            fval = (float) dptr[0];
                            coordvar->push_att(attname, fval);
                          //cout << "\tAtt " << j << ": <" << attname << "> = <" << fval << ">" << endl;
                        }
                        else
                            coordvar->push_att(attname, "do not care");
    
                        ++j;
                        step = step->next;
                    }
                }
            }
        }
    }

    delete [] coordnames;
}

