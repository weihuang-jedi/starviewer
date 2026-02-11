#ifndef _MPICOLOR_H_
#define _MPICOLOR_H_

//$Id: mpicolor.h 4862 2013-12-18 18:06:51Z starviewer $

#include <stdlib.h>

struct _color_struct
{
    short r;
    short g;
    short b;
    short a;
};

typedef struct _color_struct color_struct;

typedef enum
{
    COLOR_RED = 0,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_VIOLET,
    COLOR_FORESTGREEN,
    COLOR_PURPLE,
    COLOR_NAVY,
    COLOR_WHEAT,
    COLOR_MAGENTA,
    COLOR_MINT,
    COLOR_TEAL,
    COLOR_SKYBLUE,
    COLOR_TAN,
    COLOR_PINK,
    COLOR_ORCHID,
    COLOR_MELON,
    COLOR_BANANA,
    COLOR_CARROT,
    COLOR_RASPBERRY,
    COLOR_ORANGE,
    COLOR_GRAY,
    COLOR_LIGHTGRAY,
    COLOR_WHITE,
    NUMBEROFCOLORS
} ColorName;

class MPIColor
{
    public:
        MPIColor();
       ~MPIColor();

        color_struct get_color(ColorName colorname) { return _color[colorname]; };

        float* get_color4v(ColorName n);

    private:
        color_struct _color[NUMBEROFCOLORS];
};
#endif

