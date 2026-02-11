//$Id: radarreader.cpp 14515 2013-08-23 20:53:36Z starviewer $

#include "radarreader.h"

RADARReader::RADARReader(string flnm, string id)
{
    char fstr[1000];
    char istr[100];

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\tflnm: <" << flnm << ">, id: <" << id << ">" << endl;

    _filename = flnm;
    _id = id;

    _base_lon = 0.0;
    _base_lat = 0.0;

    verbose = false;

    wholevolume = 1;
    xdim  = 800;
    range = 800.0;
    dbz_black = 0;

    nsweeps = 0;
    nrays = 0;
    nbins = 0;

    strcpy(fstr, _filename.c_str());
    strcpy(istr, _id.c_str());

    if(verbose)
        RSL_radar_verbose_on(); // Not needed; it bides the time.

    RSL_select_fields("all", NULL);
    RSL_read_these_sweeps("all", NULL);

    radar = RSL_anyformat_to_radar(fstr, istr);

    if(NULL == radar)
    {
        cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
        cout << "\tCould not find any data for flnm: <" << flnm << ">, id: <" << id << ">" << endl;
        exit (-1);
    }

    _base_lon = (double) radar->h.lond + ((double) radar->h.lonm + (double) radar->h.lons / 60.0) / 60.0;
    _base_lat = (double) radar->h.latd + ((double) radar->h.latm + (double) radar->h.lats / 60.0) / 60.0;

  //RSL_write_radar(radar, "rsl.rsl");

    sprintf(istr,"%2.2d%2.2d%2.2d_%2.2d%2.2d",
                  radar->h.month, radar->h.day, radar->h.year-1900,
                  radar->h.hour, radar->h.minute);

    time_string = istr;

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
    cout << "\ttime_string: " << time_string << endl;
    cout << "\t_base_lon: " << _base_lon << endl;
    cout << "\t_base_lat: " << _base_lat << endl;
}

RADARReader::~RADARReader()
{
}

void RADARReader::reset()
{
    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;
} 

void RADARReader::to_gif()
{
    int i, j, n, nc;
    char r[256], g[256], b[256];
    char fname[1000];

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    for(n = 0; n < MAX_RADAR_VOLUMES; ++n)
    {
        sweep = RSL_get_first_sweep_of_volume(radar->v[n]);
        ray   = RSL_get_first_ray_of_volume(radar->v[n]);

        if(sweep)
        {
            if(SW_INDEX == n)
                RSL_load_sw_color_table();
            else if((VR_INDEX == n) || (VE_INDEX == n))
            {
                RSL_load_vel_color_table();
                RSL_rebin_velocity_volume(radar->v[n]);
            }
            else
                RSL_load_refl_color_table();

            RSL_get_color_table(RSL_RED_TABLE, r, &nc);
            RSL_get_color_table(RSL_GREEN_TABLE, g, &nc);
            RSL_get_color_table(RSL_BLUE_TABLE, b, &nc);

            for(i = 0; i < dbz_black; i++)
            {
                r[i]=(char)0;
                g[i]=(char)0;
                b[i]=(char)0;
            }

            RSL_set_color_table(RSL_RED_TABLE, r, nc);
            RSL_set_color_table(RSL_GREEN_TABLE, g, nc);
            RSL_set_color_table(RSL_BLUE_TABLE, b, nc);

            memcpy(fname,radar->h.name, sizeof(radar->h.name));
            for(j = 0; j < sizeof(radar->h.name); ++j)
                if (fname[j] == '\0' || fname[j] == ' ') break;

            if(sizeof(radar->h.name) == j)
                --j;

            cout << "\tVolume " << n << " name: " << radar->h.name << ", type: " << RSL_ftype[n] << endl;

          //_types.push_back(RSL_ftype[n]);
            _types.push_back(radar->v[n]->h.type_str);
            _nsweeps.push_back(radar->v[n]->h.nsweeps);

            if(! wholevolume)
            {
                sprintf(&fname[j], "_%s_%s.gif", RSL_ftype[n], time_string.c_str());
              //printf("FNAME = <%s>\n", fname);
                RSL_sweep_to_gif(sweep, fname, xdim, xdim, range);
              //printf("%s\n", fname);
            }
            else
            {
                sprintf(&fname[j], "_%s_%s", RSL_ftype[n], time_string.c_str());
              //RSL_volume_to_gif outputs to stdout the filenames produced.
                RSL_volume_to_gif(radar->v[n], fname, xdim, xdim, range);
            }
        }
    }
}

void RADARReader::setup()
{
    int n;
    bool notset = true;

    cout << "\nIn functions: <" << __PRETTY_FUNCTION__ << ">, line: " << __LINE__ << ", file: <" << __FILE__ << ">" << endl;

    for(n = 0; n < MAX_RADAR_VOLUMES; ++n)
    {
        sweep = RSL_get_first_sweep_of_volume(radar->v[n]);

        if(sweep)
        {
            if(notset)
            {
                ray = RSL_get_first_ray_of_volume(radar->v[n]);
                _varname = radar->v[n]->h.type_str;
                nsweeps = radar->v[n]->h.nsweeps;
                nrays = sweep->h.nrays;
                nbins = ray->h.nbins;
                notset = false;
            }

          //_types.push_back(RSL_ftype[n]);
            _types.push_back(radar->v[n]->h.type_str);
            _nsweeps.push_back(radar->v[n]->h.nsweeps);
            _sweepid.push_back(n);
        }
    }
} 

unsigned char *get_pixel_from_sweep(Sweep *s, int xdim, int ydim, float range)
{
  //Range specifies the maximum range to load that points into the image.

    int x, y;
    float azim, r;
    float val;
    int the_index;
    Ray *ray;
    float beam_width;

    unsigned char *cart_image = NULL;

    if(NULL == s)
        return NULL;

    if(xdim != ydim || ydim < 0 || xdim < 0)
    {
        fprintf(stderr, "(xdim=%d) != (ydim=%d) or either negative.\n", xdim, ydim);
        return NULL;
    }

    cart_image = (unsigned char *) calloc(xdim*ydim, sizeof(unsigned char));

    beam_width = s->h.beam_width/2.0 * 1.2;
    if(0 == beam_width)
        beam_width = 1.2;  // Sane image generation.

    for(y = -ydim/2; y < ydim/2; ++y)
    {
        for(x = -xdim/2; x < xdim/2; ++x)
        { //Find azimuth and range, then search Volume.
            if(0 != x)
                azim = (float)atan((double)y/(double)x)*180.0/3.14159;
            else
            {
                if(y < 0)
                     azim = -90.0;
                else
                    azim = 90.0;
            }

            if(y < 0 && x < 0) // Quadrant 3 (math notation).
                azim -= 180;
            else if (y >= 0 && x < 0) // Quad: 2 */
                azim += 180;

          //Radar is clockwise increasing.
            azim = -azim;

            azim -= 90.0;
            if(azim < 0)
                azim += 360.0;

            r = (float)sqrt((double)x*x + (double)y*y);
            if (ydim < xdim) r *= range/(.5*ydim);
            else r *= range/(.5*xdim);
            if(r > range)
                val = BADVAL;
            else
            {
                ray = RSL_get_closest_ray_from_sweep(s, azim, beam_width);
                val = RSL_get_value_from_ray(ray, r);
            }

            the_index =  (y+ydim/2)*ydim + (xdim-1)-(x+xdim/2);
            if(val == BADVAL || val == NOTFOUND_V || val == NOTFOUND_H)
                cart_image[the_index] = (unsigned char) 0;
            else if (val >= 0)
                cart_image[the_index] = (unsigned char) val;
            else
                cart_image[the_index] = (unsigned char) (256+val);
        }
    }

    return cart_image;
} 

Volume* RADARReader::get_volume(string vn)
{
    int n, sid = -1;

  //cout << "\nfile: " << __FILE__ << ", line: " << __LINE__ << endl;
  //cout << "\tfunction: " << __PRETTY_FUNCTION__ << endl;
  //cout << "\tget value for <" << vn << ">" << endl;

    for(n = 0; n < _types.size(); ++n)
    {
      //cout << "\t\tChecking var " << n << ": " << vn << endl;

        if(0 == vn.compare(_types[n]))
        {
          //cout << "\t\t\tFound var: " << vn << endl;
            sid = _sweepid[n];

            sweep = RSL_get_first_sweep_of_volume(radar->v[sid]);
            ray = RSL_get_first_ray_of_volume(radar->v[sid]);
            _varname = radar->v[sid]->h.type_str;
            nsweeps = radar->v[sid]->h.nsweeps;
            nrays = sweep->h.nrays;
            nbins = ray->h.nbins;

            return radar->v[sid];
        }
    }

    return NULL;
}

