//$Id: mpicolor.cpp 4862 2013-12-18 18:06:51Z starviewer $

#include "mpicolor.h"

MPIColor::MPIColor()
{
/*
    _color[COLOR_RED]         = {255,   0,   0, 255};
    _color[COLOR_GREEN]       = {  0, 255,   0, 255};
    _color[COLOR_BLUE]        = {  0,   0, 255, 255};
    _color[COLOR_YELLOW]      = {255, 255,   0, 255};
    _color[COLOR_CYAN]        = {  0, 255, 255, 255};
    _color[COLOR_MAGENTA]     = {255,   0, 255, 255};
    _color[COLOR_PINK]        = {255, 192, 203, 255};
    _color[COLOR_RASPBERRY]   = {135,  38,  87, 255};
    _color[COLOR_ORCHID]      = {218, 112, 214, 255};
    _color[COLOR_VIOLET]      = {238, 130, 238, 255};
    _color[COLOR_PURPLE]      = {128,   0, 128, 255};
    _color[COLOR_NAVY]        = {  0,   0, 128, 255};
    _color[COLOR_FORESTGREEN] = { 34, 139,  34, 255};
    _color[COLOR_TEAL]        = {0,   128, 128, 255};
    _color[COLOR_MINT]        = {189, 252, 201, 255};
    _color[COLOR_SKYBLUE]     = {135, 206, 235, 255};
    _color[COLOR_BANANA]      = {227, 207,  87, 255};
    _color[COLOR_WHEAT]       = {245, 222, 179, 255};
    _color[COLOR_TAN]         = {210, 180, 140, 255};
    _color[COLOR_MELON]       = {227, 168, 105, 255};
    _color[COLOR_CARROT]      = {237, 145,  33, 255};
    _color[COLOR_GRAY]        = { 64,  64,  64, 255};
    _color[COLOR_ORANGE]      = {255, 165,   0, 255};
    _color[COLOR_LIGHTGRAY]   = {227, 227, 227, 255};
    _color[COLOR_WHITE]       = {255, 255, 255, 255};
*/
    _color[COLOR_RED].r         = 255;
    _color[COLOR_GREEN].r       =   0;
    _color[COLOR_BLUE].r        =   0;
    _color[COLOR_YELLOW].r      = 255;
    _color[COLOR_CYAN].r        =   0;
    _color[COLOR_MAGENTA].r     = 255;
    _color[COLOR_PINK].r        = 255;
    _color[COLOR_RASPBERRY].r   = 135;
    _color[COLOR_ORCHID].r      = 218;
    _color[COLOR_VIOLET].r      = 238;
    _color[COLOR_PURPLE].r      = 128;
    _color[COLOR_NAVY].r        =   0;
    _color[COLOR_FORESTGREEN].r =  34;
    _color[COLOR_TEAL].r        =   0;
    _color[COLOR_MINT].r        = 189;
    _color[COLOR_SKYBLUE].r     = 135;
    _color[COLOR_BANANA].r      = 227;
    _color[COLOR_WHEAT].r       = 245;
    _color[COLOR_TAN].r         = 210;
    _color[COLOR_MELON].r       = 227;
    _color[COLOR_CARROT].r      = 237;
    _color[COLOR_GRAY].r        =  64;
    _color[COLOR_ORANGE].r      = 255;
    _color[COLOR_LIGHTGRAY].r   = 227;
    _color[COLOR_WHITE].r       = 255;

    _color[COLOR_RED].g         =   0;
    _color[COLOR_GREEN].g       = 255;
    _color[COLOR_BLUE].g        =   0;
    _color[COLOR_YELLOW].g      = 255;
    _color[COLOR_CYAN].g        = 255;
    _color[COLOR_MAGENTA].g     =   0;
    _color[COLOR_PINK].g        = 192;
    _color[COLOR_RASPBERRY].g   =  38;
    _color[COLOR_ORCHID].g      = 112;
    _color[COLOR_VIOLET].g      = 130;
    _color[COLOR_PURPLE].g      =   0;
    _color[COLOR_NAVY].g        =   0;
    _color[COLOR_FORESTGREEN].g = 139;
    _color[COLOR_TEAL].g        = 128;
    _color[COLOR_MINT].g        = 252;
    _color[COLOR_SKYBLUE].g     = 206;
    _color[COLOR_BANANA].g      = 207;
    _color[COLOR_WHEAT].g       = 222;
    _color[COLOR_TAN].g         = 180;
    _color[COLOR_MELON].g       = 168;
    _color[COLOR_CARROT].g      = 145;
    _color[COLOR_GRAY].g        =  64;
    _color[COLOR_ORANGE].g      = 165;
    _color[COLOR_LIGHTGRAY].g   = 227;
    _color[COLOR_WHITE].g       = 255;

    _color[COLOR_RED].b         =   0;
    _color[COLOR_GREEN].b       =   0;
    _color[COLOR_BLUE].b        = 255;
    _color[COLOR_YELLOW].b      =   0;
    _color[COLOR_CYAN].b        = 255;
    _color[COLOR_MAGENTA].b     = 255;
    _color[COLOR_PINK].b        = 203;
    _color[COLOR_RASPBERRY].b   =  87;
    _color[COLOR_ORCHID].b      = 214;
    _color[COLOR_VIOLET].b      = 238;
    _color[COLOR_PURPLE].b      = 128;
    _color[COLOR_NAVY].b        = 128;
    _color[COLOR_FORESTGREEN].b =  34;
    _color[COLOR_TEAL].b        =   0;
    _color[COLOR_MINT].b        = 201;
    _color[COLOR_SKYBLUE].b     = 235;
    _color[COLOR_BANANA].b      =  87;
    _color[COLOR_WHEAT].b       = 179;
    _color[COLOR_TAN].b         = 140;
    _color[COLOR_MELON].b       = 105;
    _color[COLOR_CARROT].b      =  33;
    _color[COLOR_GRAY].b        =  64;
    _color[COLOR_ORANGE].b      =   0;
    _color[COLOR_LIGHTGRAY].b   = 227;
    _color[COLOR_WHITE].b       = 255;

    _color[COLOR_RED].a         = 255;
    _color[COLOR_GREEN].a       = 255;
    _color[COLOR_BLUE].a        = 255;
    _color[COLOR_YELLOW].a      = 255;
    _color[COLOR_CYAN].a        = 255;
    _color[COLOR_MAGENTA].a     = 255;
    _color[COLOR_PINK].a        = 255;
    _color[COLOR_RASPBERRY].a   = 255;
    _color[COLOR_ORCHID].a      = 255;
    _color[COLOR_VIOLET].a      = 255;
    _color[COLOR_PURPLE].a      = 255;
    _color[COLOR_NAVY].a        = 255;
    _color[COLOR_FORESTGREEN].a = 255;
    _color[COLOR_TEAL].a        = 255;
    _color[COLOR_MINT].a        = 255;
    _color[COLOR_SKYBLUE].a     = 255;
    _color[COLOR_BANANA].a      = 255;
    _color[COLOR_WHEAT].a       = 255;
    _color[COLOR_TAN].a         = 255;
    _color[COLOR_MELON].a       = 255;
    _color[COLOR_CARROT].a      = 255;
    _color[COLOR_GRAY].a        = 255;
    _color[COLOR_ORANGE].a      = 255;
    _color[COLOR_LIGHTGRAY].a   = 255;
    _color[COLOR_WHITE].a       = 255;
}

MPIColor::~MPIColor()
{
}

float* MPIColor::get_color4v(ColorName n)
{
    float* c4v = new float[4];
    c4v[0] = _color[n].r/255.0;
    c4v[1] = _color[n].g/255.0;
    c4v[2] = _color[n].b/255.0;
    c4v[3] = _color[n].a/255.0;
    return c4v;
}

