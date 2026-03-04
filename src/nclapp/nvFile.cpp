#include "nvFile.h"

NVFile::NVFile(string flnm, bool isList, bool isWRFmodel)
{
    WRFmodel = isWRFmodel;

    _setup(flnm, isList);
}

NVFile::NVFile(string flnm, bool isList)
{
    WRFmodel = false;

    _setup(flnm, isList);
}

NVFile::~NVFile()
{
   if(nfils > 0)
   {
       delete [] ntimes;
       delete [] filehandler;
   }

   if(NULL != timetitle)
       delete [] timetitle;

 //finalizeNcl();
}

void NVFile::_setup(string flnm, bool isList)
{
    int n;

    string fn;

  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\t\tflnm = " << flnm << endl;

    natts = 0;
    ndims = 0;
    nvars = 0;
    nfils = 0;
    ncoords = 0;

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    prevfile = -1;
    currfile = 0;

    hasTime = false;
    hasTimeVar = false;
    timevalue = NULL;

    has1dLon = false;
    has1dLat = false;
    has1dLev = false;

    has2dLon = false;
    has2dLat = false;

    lon_value = NULL;
    lat_value = NULL;
    lev_value = NULL;

    timetitle = NULL;
    
    set_flnm(flnm);

    if(isList)
    {
       _read_filelist();
    }
    else
    {
        nfils = 1;
        fileList.push_back(flnm);
    }

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\tnfils: " << nfils << endl;

    initializeNcl();

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\tnfils: " << nfils << endl;

    filehandler = new NclFileHandler[nfils];
    ntimes = new int[nfils];

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    timevarname = "unknown";

    for(n = 0; n < nfils; ++n)
    {
      //cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;
      //cout << "\tfileList[" << n << "] = " << fileList[n] << endl;
        fn = fileList[n];
        filehandler[n].open(fn);
        ntimes[n] = _get_file_time_size(n);
    }

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    _set_file_property();

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void NVFile::_set_file_property()
{
    int n, nd, ndmax;

    Dimension* vdims = NULL;

  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\t\tcurrfile = " << currfile << endl;

    natts = filehandler[currfile].get_natts();
    ndims = filehandler[currfile].get_ndims();
    nvars = filehandler[currfile].get_nvars();
    ncoords = filehandler[currfile].get_ncoords();

    atts = filehandler[currfile].get_atts();
    dims = filehandler[currfile].get_dims();
    vars = filehandler[currfile].get_vars();
    coordvars = filehandler[currfile].get_coordvars();

  //cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    domsize[0] = 1;
    domsize[1] = 1;
    domsize[2] = 1;

  //cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _check_coords();

  //cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    if((!has1dLon) && (!has1dLat) &&
       (!has2dLon) && (!has2dLat))
        _recheck_coords();

  //cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    if(2 > domsize[2])
    {
        ndmax = 0;
        for(n = 0; n < nvars; ++n)
        {
            vdims = vars[n].get_dims();
            nd = vars[n].get_ndims();
            if(nd <= ndmax)
                continue;
            if(nd <= 1)
                continue;

            ndmax = nd;
            switch (nd)
            {
                case 1:
                       domsize[2] = vdims[0].get_size();
                       break;
                case 2:
                       domsize[2] = vdims[1].get_size();
                       domsize[1] = vdims[0].get_size();
                       break;
                case 3:
                       domsize[2] = vdims[2].get_size();
                       domsize[1] = vdims[1].get_size();
                       domsize[0] = vdims[0].get_size();
                       ntimes[currfile] = 1;
                       break;
                default:
                       domsize[2] = vdims[nd - 1].get_size();
                       domsize[1] = vdims[nd - 2].get_size();
                       domsize[0] = vdims[nd - 3].get_size();
                       ntimes[currfile]  = vdims[nd - 4].get_size();
                       hasTime = true;
                       break;
            }
        }
    }

  //cout << "\tfile: " << __FILE__ << ", line: " << __LINE__ << endl;

    _build_timestring(true);

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

Variable* NVFile::get_var(int n)
{
    return &(vars[n]);
}

Variable* NVFile::get_varWithName(string vn)
{
    Variable* var;
    int n;

    for(n = 0; n < nvars; ++n)
    {
        var = &(vars[n]);
        if(0 == vn.compare(var->get_name()))
            return var;
    }

    return NULL;
}

Dimension* NVFile::get_dim(int n)
{
    return &(dims[n]);
}

Dimension* NVFile::get_dimWithName(string dn)
{
    Dimension* dim;
    int n;

    for(n = 0; n < ndims; ++n)
    {
        dim = &(dims[n]);
        if(0 == dn.compare(dim->get_name()))
            return dim;
    }

    return NULL;
}

Attribute* NVFile::get_att(int n)
{
    return &(atts[n]);
}

Attribute* NVFile::get_attWithName(string an)
{
    Attribute* att;
    int n;

    for(n = 0; n < natts; ++n)
    {
        att = &(atts[n]);
        if(0 == an.compare(att->get_name()))
            return att;
    }

    return NULL;
}

void NVFile::_read_filelist()
{
    ifstream inFile;
    string contents = "BLANK";

  //cout << "\nFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tflnm: <" << flnm << ">" << endl;

    inFile.open(flnm.c_str(), ios::in | ios::binary);

    if(! inFile.is_open())
    {
        inFile.close();
        throw(errno);
    }

    nfils = 0;
    while(getline(inFile, contents))
    {
        ++nfils;
        fileList.push_back(contents);

      //cout << "\tFile " << nfils << ": <" << contents << ">" << endl;
    }

    inFile.close();
}

string NVFile::get_varname(int n)
{
    int i, nd;
    string varname = "unknown";

  //cout << "\nFunciton: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\thasTime: <" << hasTime << ">, nvars = " << nvars << endl;

    for(i = 0; i < nvars; ++i)
    {
        nd = vars[i].get_ndims();

      //cout << "\tvar " << i << ": <" << vars[i].get_name() << ">, nd = " << vars[i].get_ndims() << endl;

        if(hasTime)
        {
            if(n == (nd-1))
            {
                varname = vars[i].get_name();

              //cout << "\nFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\tvarname: <" << varname << ">" << endl;
              //cout << "\tnd = " << nd << endl;

                break;
            }
        }
        else
        {
            if(n == nd)
            {
                varname = vars[i].get_name();

              //cout << "\nFunciton: " << __PRETTY_FUNCTION__
              //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\tvarname: <" << varname << ">" << endl;
              //cout << "\tnd = " << nd << endl;

                break;
            }
        }
    }

    return varname;
}

float* NVFile::get_fv(string vn)
{
    int i;
    int nd;

    string attname;
    string attvalue;

    string dimname;
    int    dimsize;

    Variable* var = get_varWithName(vn);
    Attribute* vatts = var->get_atts();
    Dimension* vdims = var->get_dims();
    int nvdims = var->get_ndims();

    float* value;

    title = vn;

    for(i = 0; i < var->get_natts(); ++i)
    {
        attname = vatts[i].get_name();
        attvalue = vatts[i].get_value();
        if(0 == attname.compare("long_name"))
        {
           title.append("       long_name: ");
           title.append(attvalue);
        }
        else if(0 == attname.compare("units"))
        {
           title.append("       units: ");
           title.append(attvalue);
        }
      //else if(0 == attname.compare("coordinates"))
      //{
      //   coordstring = attvalue;
      //}
    }

  //cout << "\nFunciton: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\ttitle: <" << title << ">" << endl;

    varsize[0] = 1;
    varsize[1] = 1;
    varsize[2] = 1;

    for(i = 0; i < nvdims; ++i)
    {
        dimname = vdims[i].get_name();
        dimsize = vdims[i].get_size();

        if(0 == dimname.compare("lon"))
            varsize[2] = dimsize;
        else if(0 == dimname.compare("lat"))
            varsize[1] = dimsize;
        else if(0 == dimname.compare("lev"))
            varsize[0] = dimsize;
    }

    if(2 > varsize[2])
    {
        nd = var->get_ndims();
        if(1 >= nd)
        {
            varsize[2] = vdims[0].get_size();
        }
        if(2 == nd)
        {
            varsize[2] = vdims[1].get_size();
            varsize[1] = vdims[0].get_size();
        }
        else if(3 <= nd)
        {
            varsize[2] = vdims[nd - 1].get_size();
            varsize[1] = vdims[nd - 2].get_size();
            varsize[0] = vdims[nd - 3].get_size();
        }
    }

  //cout << "\nFunciton: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tvarsize[0]: <" << varsize[0] << ">" << endl;
  //cout << "\tvarsize[1]: <" << varsize[1] << ">" << endl;
  //cout << "\tvarsize[2]: <" << varsize[2] << ">" << endl;

    if(var->get_hasFillValue())
    {
        hasFillValue = true;
        fillValue = var->get_fillValue();
    }
    else
    {
        hasFillValue = false;
        fillValue = 1.0e20;
    }

    value = filehandler[currfile].get_fv(vn);
    return value;
}

int* NVFile::get_varsize()
{
    return varsize;
}

int NVFile::get_ndv(int n)
{
    return filehandler[currfile].get_ndv(n);
}

string* NVFile::get_ndvNames(int n)
{
    string* varnames = filehandler[currfile].get_ndvNames(n);
    return varnames;
}

string* NVFile::get_timestr()
{
    _build_timestring(false);
    return timetitle;
}

void NVFile::_build_timestring(bool first)
{
    int n = 0;
    int nstart = 0;
    stringstream ss;

    string attname;
    string attvalue;
    string units = " ";

  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

    if(!first)
    {
        if(NULL != timetitle)
            delete [] timetitle;
    }

  //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\thasTimeVar: <" << hasTimeVar << ">" << endl;
  //cout << "\tfirst: <" << first << ">" << endl;

    if(hasTimeVar)
    {
      //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

        Variable* var = get_varWithName(timevarname);
        Attribute* vatts = var->get_atts();

      //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

        if(!first)
        {
            if(NULL != timevalue)
                free(timevalue);
        }

      //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;

        if(WRFmodel)
            timevalue = get_dv(string("XTIME"));
        else
            timevalue = get_dv(string("time"));

      //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
      //cout << "\ttimevalue: <" << timevalue << ">" << endl;

        timetitle = new string[ntimes[currfile]];

        for(n = 0; n < var->get_natts(); ++n)
        {
            attname = vatts[n].get_name();
            attvalue = vatts[n].get_value();
            if(0 == attname.compare("units"))
            {
               units = attvalue;
               break;
            }
        }

      //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
      //cout << "\tntimes[currfile] = " << ntimes[currfile] << endl;

        for(n = 0; n < ntimes[currfile]; ++n)
        {
            ss.str("");
            ss << timevalue[n];

          //cout << "\tss.str() = " << ss.str() << endl;

            timetitle[n] = "Time: ";
            timetitle[n].append(ss.str());
            timetitle[n].append("      ");
            timetitle[n].append(units);

          //cout << "\ttimetitle[" << n << "] = " << timetitle[n] << endl;
        }
    }
    else
    {
        timetitle = new string[ntimes[currfile]];

      //cout << "\tfunction: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
      //cout << "\tntimes[currfile] = " << ntimes[currfile] << endl;

        nstart = 0;
        for(n = 0; n < currfile; ++n)
        {
          //cout << "\tntimes[" << n << "] = " << ntimes[n] << endl;
            nstart += ntimes[n];
        }

        for(n = 0; n < ntimes[currfile]; ++n)
        {
            ss.str("");
            ss << n + nstart;

          //cout << "\tss.str() = " << ss.str() << endl;

            timetitle[n] = "Time: ";
            timetitle[n].append(ss.str());

          //cout << "\ttimetitle[" << n << "] = " << timetitle[n] << endl;
        }
    }

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}

void NVFile::select_file(int nf)
{
    if(nf >= nfils)
        currfile = 0;
    else
        currfile = nf;

    if(prevfile == currfile)
        return;
    _set_file_property();
    prevfile = currfile;
}

double* NVFile::get_dv(string vn)
{
    int i;
    int nd;

    string attname;
    string attvalue;

    string dimname;
    int    dimsize;

    Variable* var = get_varWithName(vn);
    Attribute* vatts = var->get_atts();
    Dimension* vdims = var->get_dims();
    int nvdims = var->get_ndims();

    double* value;

    title = vn;

    for(i = 0; i < var->get_natts(); ++i)
    {
        attname = vatts[i].get_name();
        attvalue = vatts[i].get_value();
        if(0 == attname.compare("long_name"))
        {
           title.append("       long_name: ");
           title.append(attvalue);
        }
        else if(0 == attname.compare("units"))
        {
           title.append("       units: ");
           title.append(attvalue);
        }
    }

  //cout << "\nFunciton: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\ttitle: <" << title << ">" << endl;

    varsize[0] = 1;
    varsize[1] = 1;
    varsize[2] = 1;

    for(i = 0; i < nvdims; ++i)
    {
        dimname = vdims[i].get_name();
        dimsize = vdims[i].get_size();

        if(0 == dimname.compare("lon"))
            varsize[2] = dimsize;
        else if(0 == dimname.compare("lat"))
            varsize[1] = dimsize;
        else if(0 == dimname.compare("lev"))
            varsize[0] = dimsize;
    }

    if(2 > varsize[2])
    {
        nd = var->get_ndims();
        if(1 >= nd)
        {
            varsize[2] = vdims[0].get_size();
        }
        if(2 == nd)
        {
            varsize[2] = vdims[1].get_size();
            varsize[1] = vdims[0].get_size();
        }
        else if(3 <= nd)
        {
            varsize[2] = vdims[nd - 1].get_size();
            varsize[1] = vdims[nd - 2].get_size();
            varsize[0] = vdims[nd - 3].get_size();
        }
    }

  //cout << "\nFunciton: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tvarsize[0]: <" << varsize[0] << ">" << endl;
  //cout << "\tvarsize[1]: <" << varsize[1] << ">" << endl;
  //cout << "\tvarsize[2]: <" << varsize[2] << ">" << endl;

    if(var->get_hasFillValue())
    {
        hasFillValue = true;
        fillValue = var->get_fillValue();
    }
    else
    {
        hasFillValue = false;
        fillValue = 1.0e20;
    }

    value = filehandler[currfile].get_dv(vn);
    return value;
}

int* NVFile::get_iv(string vn)
{
    int* value;

    value = filehandler[currfile].get_iv(vn);
    return value;
}

string NVFile::get_sv(string vn)
{
    string value;

    value = filehandler[currfile].get_sv(vn);
    return value;
}

int NVFile::get_dim_size(const char* dn)
{
    int i;

    for(i = 0; i < ndims; ++i)
    {
        if(0 == dims[i].get_name().compare(dn))
        {
            return dims[i].get_size();
        }
    }

    return 0;
}

int NVFile::_get_file_time_size(int n)
{
    int i;
    int ntmp;
    Dimension* tmpdims;


    ntmp = filehandler[n].get_ndims();
    tmpdims = filehandler[n].get_dims();

    for(i = 0; i < ntmp; ++i)
    {
        if(0 == tmpdims[i].get_name().compare("time"))
        {
            return (tmpdims[i].get_size());
        }
        else if(0 == tmpdims[i].get_name().compare("Time"))
        {
            return (tmpdims[i].get_size());
        }
    }

    return 1;
}

string NVFile::get_att_value(const char* cn)
{
    return get_att_value(string(cn));
}

string NVFile::get_att_value(string an)
{
    int i;
    string attname;
    string attvalue = "unknown";

    natts = filehandler[currfile].get_natts();
    atts  = filehandler[currfile].get_atts();

    for(i = 0; i < natts; ++i)
    {
        attname = atts[i].get_name();
        if(0 == attname.compare(an))
        {
            attvalue = atts[i].get_value();
            return attvalue;
        }
    }

    return attvalue;
}

string NVFile::toLowerString(string is)
{
    string ls;
    char cstr[256];
    size_t n;

    strcpy(cstr, is.c_str());

    for(n = 0; n < is.size(); ++n)
        cstr[n] = tolower(cstr[n]);

    ls = cstr;
    return ls;
}

void NVFile::_check_coords()
{
    int n;
    int ndims;
    size_t pos;
    string dimname;
    string varname;
    string lowcoordname;

    Variable* var = NULL;
    Dimension* vdims = NULL;

  //bool found_lon = false;
  //bool found_lat = false;
  //bool found_lev = false;
  //bool found_time = false;

  //cout << "\tEnter function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
  //cout << "\tncoords = " << ncoords << endl;

    for(n = 0; n < ncoords; ++n)
    {
        var = &(coordvars[n]);
        varname = var->get_name();

        if((0 == varname.compare("XLONG_U")) ||
           (0 == varname.compare("XLONG_V")) ||
           (0 == varname.compare("XLAT_U")) ||
           (0 == varname.compare("XLAT_V")))
            continue;

        lowcoordname = toLowerString(var->get_name());

      //cout << "\tcoords " << n << ": " << var->get_name() << ", lowcoordname.size() = " << lowcoordname.size() << endl;
      //cout << "\tlowcoordname = " << lowcoordname << endl;

        vdims = var->get_dims();
        ndims = var->get_ndims();

      //cout << "\t\tndims = " << ndims << endl;

        pos = lowcoordname.find("lon");
      //cout << "\t\tlon pos = " << pos << endl;

        if(lowcoordname.size() > pos)
        {
            if(1 == ndims)
            {
                domsize[2] = vdims[0].get_size();
                if(!has1dLon)
                    lon_value = filehandler[currfile].get_dv(var->get_name());
                has1dLon = true;
            }
            else if(2 == ndims)
            {
                if(!has2dLon)
                    lon_value = filehandler[currfile].get_dv(var->get_name());
                has2dLon = true;
            }
            else
            {
                pos = lowcoordname.find("xlong");
              //cout << "\t\tlon pos = " << pos << ", lowcoordname.size() = " << lowcoordname.size() << endl;

                if(lowcoordname.size() > pos)
                {
                    if(!has2dLon)
                        lon_value = filehandler[currfile].get_dv(var->get_name());
                    has2dLon = true;
                }

              //cout << "\nFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\tcoords " << n << ": " << var->get_name() << endl;
              //cout << "\t\tlon pos = " << pos << ", ndims = " << ndims << endl;
            }
            continue;
        }

        pos = lowcoordname.find("lat");
      //cout << "\t\tlat pos = " << pos << endl;

        if(lowcoordname.size() > pos)
        {
            if(1 == ndims)
            {
                domsize[1] = vdims[0].get_size();
                if(!has1dLat)
                    lat_value = filehandler[currfile].get_dv(var->get_name());
                has1dLat = true;
            }
            else if(2 == ndims)
            {
                if(!has2dLat)
                    lat_value = filehandler[currfile].get_dv(var->get_name());
                has2dLat = true;
            }
            else
            {
                pos = lowcoordname.find("xlat");
              //cout << "\t\tlat pos = " << pos << endl;

                if(lowcoordname.size() > pos)
                {
                    if(!has2dLat)
                        lat_value = filehandler[currfile].get_dv(var->get_name());
                    has2dLat = true;
                }

              //cout << "\tFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\tcoords " << n << ": " << var->get_name() << endl;
              //cout << "\t\tlat pos = " << pos << ", ndims = " << ndims << endl;
            }
            continue;
        }

        pos = lowcoordname.find("lev");
      //cout << "\t\tlev pos = " << pos << endl;

        if(lowcoordname.size() > pos)
        {
            if(1 == ndims)
            {
                domsize[0] = vdims[0].get_size();
                if(!has1dLev)
                    lev_value = filehandler[currfile].get_dv(var->get_name());
                has1dLev = true;
            }
            else
            {
              //cout << "\nFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
              //cout << "\tcoords " << n << ": " << var->get_name() << endl;
              //cout << "\t\tlev pos = " << pos << endl;
            }
            continue;
        }

        pos = lowcoordname.find("time");
      //cout << "\t\ttime pos = " << pos << endl;

        if(lowcoordname.size() > pos)
        {
            if(1 == ndims)
            {
                ntimes[currfile] = vdims[0].get_size();
                timevarname = var->get_name();
                timevalue = filehandler[currfile].get_dv(var->get_name());
                hasTime = true;
                hasTimeVar = true;
            }
            else
            {
                cout << "\nFunciton: " << __PRETTY_FUNCTION__ << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;
                cout << "\tcoords " << n << ": " << var->get_name() << endl;
                cout << "\t\ttime pos = " << pos << endl;
            }
        }

#if 1
        int i;
        string dimname;
        int    dimsize;

        for(i = 0; i < ndims; ++i)
        {
            dimname = vdims[i].get_name();
            dimsize = vdims[i].get_size();

          //cout << "\t\tdim " << i << " name: <" << dimname << ">, size = " << dimsize << endl;
        }
#endif
    }

  //cout << "\tLeave function: <" << __PRETTY_FUNCTION__ << ">, in file: <" << __FILE__ << ">, at line: " << __LINE__ << endl;
}


bool NVFile::_isVar(string vn)
{
    int i;

  //cout << "\nFunciton: " << __PRETTY_FUNCTION__
  //     << ", file: " << __FILE__ << ", line: " << __LINE__ << endl;

    for(i = 0; i < nvars; ++i)
    {
        if(0 == vn.compare(vars[i].get_name()))
            return true;
    }

    return false;
}

void NVFile::_recheck_coords()
{
    int i;
    size_t pos;
    string dimname;
    string lowdimname;

    for(i = 0; i < ndims; ++i)
    {
        dimname = dims[i].get_name();
        lowdimname = toLowerString(dimname);

        pos = lowdimname.find("lon");

        if(lowdimname.size() > pos)
        {
            domsize[2] = dims[i].get_size();
            if(_isVar(dimname))
            {
                if(!has1dLon)
                    lon_value = filehandler[currfile].get_dv(dimname);
                has1dLon = true;
            }

            continue;
        }

        pos = lowdimname.find("lat");

        if(lowdimname.size() > pos)
        {
            domsize[1] = dims[i].get_size();
            if(_isVar(dimname))
            {
                if(!has1dLat)
                    lat_value = filehandler[currfile].get_dv(dimname);
                has1dLat = true;
            }

            continue;
        }

        pos = lowdimname.find("lev");

        if(lowdimname.size() > pos)
        {
            domsize[0] = dims[i].get_size();
            if(_isVar(dimname))
            {
                if(!has1dLev)
                    lev_value = filehandler[currfile].get_dv(dimname);
                has1dLev = true;
            }

            continue;
        }

        pos = lowdimname.find("time");

        if(lowdimname.size() > pos)
        {
            if(_isVar(dimname))
            {
                ntimes[currfile] = dims[i].get_size();
                timevarname = dimname;
                timevalue = filehandler[currfile].get_dv(dimname);
                hasTime = true;
                hasTimeVar = true;
            }

            continue;
        }
    }
}

