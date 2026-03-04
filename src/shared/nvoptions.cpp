//$Id: nvoptions.cpp 5191 2014-08-15 20:45:14Z starviewer $

#include "nvoptions.h"

NVOptions::NVOptions()
{
    int n;

    cb_options = new bool[NV_NUMBEROFCBS];

    for(n = 0; n < NV_NUMBEROFCBS; ++n)
        cb_options[n] = false;

    set_cb(NV_ISPERSPECTIVE, true);
    set_cb(NV_TITLEON, true);
    set_cb(NV_COASTLINEON, true);

    model = NOMODEL;

    numberShaderPasses = -1;

    xpos = -1;
    ypos = -1;
    zpos = -1;
    tpos = 0;

    xsec = -1;
    ysec = -1;
    zsec = -1;
    tsec = 0;

    xsec2 = -1;
    ysec2 = -1;
    zsec2 = -1;
    tsec2 = -1;

    _varname = "unknown";
    _title = "unknown";
    _timeinfo = "unknown";
    _position = "unknown";
}

NVOptions::~NVOptions()
{
    delete [] cb_options;
}

void NVOptions::set_cb(NV_cbType cb, bool v)
{
    cb_options[cb] = v;
}

bool NVOptions::get_cb(NV_cbType cb)
{
    return cb_options[cb];
}

