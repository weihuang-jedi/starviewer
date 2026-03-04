#include <QOpenGLWidget>
#include <color.h>

//
//  Constructor
//
Color::Color()
{
    _nc = 0;
    _max_nc = 255;

    _color_field = (color_field_struct *)calloc(_max_nc, sizeof(color_field_struct));

    _set_color_field("Black",	0.0, 0.0, 0.0);
    _set_color_field("White",	1.0, 1.0, 1.0);

    _set_color_field("Red",	1.0, 0.0, 0.0);
    _set_color_field("Green",	0.0, 1.0, 0.0);
    _set_color_field("Blue",	0.0, 0.0, 1.0);
    _set_color_field("Yellow",	1.0, 1.0, 0.0);
    _set_color_field("Cyan",	0.0, 1.0, 1.0);
    _set_color_field("Magenta",	1.0, 0.0, 1.0);
    _set_color_field("Clear",	1.0, 1.0, 1.0);

    _set_color_field("Gray05",	0.05, 0.05, 0.05);
    _set_color_field("Gray10",	0.10, 0.10, 0.10);
    _set_color_field("Gray15",	0.15, 0.15, 0.15);
    _set_color_field("Gray20",	0.20, 0.20, 0.20);
    _set_color_field("Gray25",	0.25, 0.25, 0.25);
    _set_color_field("Gray30",	0.30, 0.30, 0.30);
    _set_color_field("Gray35",	0.35, 0.35, 0.35);
    _set_color_field("Gray40",	0.40, 0.40, 0.40);
    _set_color_field("Gray45",	0.45, 0.45, 0.45);
    _set_color_field("Gray50",	0.50, 0.50, 0.50);
    _set_color_field("Gray55",	0.55, 0.55, 0.55);
    _set_color_field("Gray60",	0.60, 0.60, 0.60);
    _set_color_field("Gray65",	0.65, 0.65, 0.65);
    _set_color_field("Gray70",	0.70, 0.70, 0.70);
    _set_color_field("Gray75",	0.75, 0.75, 0.75);
    _set_color_field("Gray80",	0.80, 0.80, 0.80);
    _set_color_field("Gray85",	0.85, 0.85, 0.85);
    _set_color_field("Gray90",	0.90, 0.90, 0.90);
    _set_color_field("Gray95",	0.95, 0.95, 0.95);

    _set_color_field("DimGrey",		0.329412, 0.329412, 0.329412);
    _set_color_field("Grey",		0.752941, 0.752941, 0.752941);
    _set_color_field("LightGray",	0.658824, 0.658824, 0.658824);
    _set_color_field("VLightGray",	0.80,     0.80,     0.80);

    _set_color_field("Aquamarine",	0.439216, 0.858824, 0.576471);
    _set_color_field("BlueViolet",	0.62352,  0.372549, 0.623529);
    _set_color_field("Brown",		0.647059, 0.164706, 0.164706);
    _set_color_field("CadetBlue",	0.372549, 0.623529, 0.623529);
    _set_color_field("Coral",		1.0,      0.498039, 0.0);
    _set_color_field("CornflowerBlue",	0.258824, 0.258824, 0.435294);
    _set_color_field("DarkGreen",	0.184314, 0.309804, 0.184314);
    _set_color_field("DarkOliveGreen",	0.309804, 0.309804, 0.184314);
    _set_color_field("DarkOrchid",	0.6,      0.196078, 0.8);
    _set_color_field("DarkSlateBlue",	0.419608, 0.137255, 0.556863);
    _set_color_field("DarkSlateGray",	0.184314, 0.309804, 0.309804);
    _set_color_field("DarkSlateGrey",	0.184314, 0.309804, 0.309804);
    _set_color_field("DarkTurquoise",	0.439216, 0.576471, 0.858824);
    _set_color_field("Firebrick",	0.556863, 0.137255, 0.137255);
    _set_color_field("ForestGreen",	0.137255, 0.556863, 0.137255);
    _set_color_field("Gold",		0.8,      0.498039, 0.196078);
    _set_color_field("Goldenrod",	0.858824, 0.858824, 0.439216);
    _set_color_field("GreenYellow",	0.576471, 0.858824, 0.439216);
    _set_color_field("IndianRed",	0.309804, 0.184314, 0.184314);
    _set_color_field("Khaki",		0.623529, 0.623529, 0.372549);
    _set_color_field("LightBlue",	0.74902,  0.847059, 0.847059);
    _set_color_field("LightSteelBlue",	0.560784, 0.560784, 0.737255);
    _set_color_field("LimeGreen",	0.196078, 0.8,      0.196078);
    _set_color_field("Maroon",		0.556863, 0.137255, 0.419608);
    _set_color_field("MediumAquamarine",0.196078, 0.8,     0.6);
    _set_color_field("MediumBlue",	0.196078, 0.196078, 0.8);
    _set_color_field("MediumForestGreen",0.41961, 0.556863, 0.137255);
    _set_color_field("MediumGoldenrod", 0.917647, 0.917647, 0.678431);
    _set_color_field("MediumOrchid",	0.576471, 0.439216, 0.858824);
    _set_color_field("MediumSeaGreen",	0.258824, 0.435294, 0.258824);
    _set_color_field("MediumSlateBlue", 0.0,      0.498039, 1.0);
    _set_color_field("MediumSpringGreen",0.49804, 1.0, 0.0);
    _set_color_field("MediumTurquoise", 0.439216, 0.858824, 0.858824);
    _set_color_field("MediumVioletRed", 0.858824, 0.439216, 0.576471);
    _set_color_field("MidnightBlue",	0.184314, 0.184314, 0.309804);
    _set_color_field("Navy",		0.137255, 0.137255, 0.556863);
    _set_color_field("NavyBlue",	0.137255, 0.137255, 0.556863);
    _set_color_field("Orange",		1.0,      0.5,      0.0);
    _set_color_field("OrangeRed",	1.0,      0.25,     0.0);
    _set_color_field("Orchid",		0.858824, 0.439216, 0.858824);
    _set_color_field("PaleGreen",	0.560784, 0.737255, 0.560784);
    _set_color_field("Pink",		0.737255, 0.560784, 0.560784);
    _set_color_field("Plum",		0.917647, 0.678431, 0.917647);
    _set_color_field("Salmon",		0.435294, 0.258824, 0.258824);
    _set_color_field("SeaGreen",	0.137255, 0.556863, 0.419608);
    _set_color_field("Sienna",		0.556863, 0.419608, 0.137255);
    _set_color_field("SkyBlue",		0.196078, 0.6,      0.8);
    _set_color_field("SlateBlue",	0.0,      0.498039, 1.0);
    _set_color_field("SpringGreen",	0.0,      1.0,      0.498039);
    _set_color_field("SteelBlue",	0.137255, 0.419608, 0.556863);
    _set_color_field("Tan",		0.858824, 0.576471, 0.439216);
    _set_color_field("Thistle",		0.847059, 0.74902,  0.847059);
    _set_color_field("Turquoise",	0.678431, 0.917647, 0.917647);
    _set_color_field("Violet",		0.309804, 0.184314, 0.309804);
    _set_color_field("VioletRed",	0.8,      0.196078, 0.6);
    _set_color_field("Wheat",		0.847059, 0.847059, 0.74902);
    _set_color_field("YellowGreen",	0.6,      0.8,      0.196078);

    _set_color_field("SummerSky",	0.22, 0.69, 0.87);
    _set_color_field("RichBlue",	0.35, 0.35, 0.67);
    _set_color_field("Brass",		0.71, 0.65, 0.26);
    _set_color_field("Copper",		0.72, 0.45, 0.20);
    _set_color_field("Bronze",		0.55, 0.47, 0.14);
    _set_color_field("Bronze2",		0.65, 0.49, 0.24);
    _set_color_field("Silver",		0.90, 0.91, 0.98);
    _set_color_field("BrightGold",	0.85, 0.85, 0.10);
    _set_color_field("OldGold",		0.81, 0.71, 0.23);
    _set_color_field("Feldspar",	0.82, 0.57, 0.46);
    _set_color_field("Quartz",		0.85, 0.85, 0.95);
    _set_color_field("Mica",		0.00, 0.00, 0.00);
    _set_color_field("NeonPink",	1.00, 0.43, 0.78);
    _set_color_field("DarkPurple",	0.53, 0.12, 0.47);
    _set_color_field("NeonBlue",	0.30, 0.30, 1.00);
    _set_color_field("CoolCopper",	0.85, 0.53, 0.10);
    _set_color_field("MandarinOrange",	0.89, 0.47, 0.20);
    _set_color_field("LightWood",	0.91, 0.76, 0.65);
    _set_color_field("MediumWood",	0.65, 0.50, 0.39);
    _set_color_field("DarkWood",	0.52, 0.37, 0.26);
    _set_color_field("SpicyPink",	1.00, 0.11, 0.68);
    _set_color_field("SemiSweetChoc",	0.42, 0.26, 0.15);
    _set_color_field("BakersChoc",	0.36, 0.20, 0.09);
    _set_color_field("Flesh",		0.96, 0.80, 0.69);
    _set_color_field("NewTan",		0.92, 0.78, 0.62);
    _set_color_field("NewMidnightBlue", 0.00, 0.00, 0.61);
    _set_color_field("VeryDarkBrown",	0.35, 0.16, 0.14);
    _set_color_field("DarkBrown",	0.36, 0.25, 0.20);
    _set_color_field("DarkTan",		0.59, 0.41, 0.31);
    _set_color_field("GreenCopper",	0.32, 0.49, 0.46);
    _set_color_field("DkGreenCopper",	0.29, 0.46, 0.43);
    _set_color_field("DustyRose",	0.52, 0.39, 0.39);
    _set_color_field("HuntersGreen",	0.13, 0.37, 0.31);
    _set_color_field("Scarlet",		0.55, 0.09, 0.09);

    _set_color_field("Med_Purple",	 0.73, 0.16, 0.96);
    _set_color_field("Light_Purple",	 0.87, 0.58, 0.98);
    _set_color_field("Very_Light_Purple",0.94, 0.81, 0.99);
}

Color::~Color()
{
    if(0 < _nc)
        free(_color_field);
}

void Color::_set_color_field(const char *name,
                             GLfloat r, GLfloat g, GLfloat b)
{
    int n;

    if(_nc >= _max_nc)
    {
        fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stderr, "\t_nc = %d, _max_nc = %d, can not add <%s>\n",
                           _nc, _max_nc, name);

        return;

        _max_nc *= 2;

        _color_field = (color_field_struct *)
                        realloc(_color_field,
                                _max_nc * sizeof(color_field_struct));
        for(n = _nc; n < _max_nc; n ++)
        {
            strcpy(_color_field[n].name, "NoName");
            memset(_color_field[n].val, 0, 3 * sizeof(GLfloat));
        }
    }

    if(NULL != name)
        strcpy(_color_field[_nc].name, name);
    else
        strcpy(_color_field[_nc].name, "NoName");
    
    _color_field[_nc].val[0] = r;
    _color_field[_nc].val[1] = g;
    _color_field[_nc].val[2] = b;

    _nc++;
}

void Color::set_color(const char *name, GLfloat r, GLfloat g, GLfloat b)
{
    _set_color_field(name, r, g, b);
}

GLfloat *Color::get_val_by_name(const char *name)
{
    int n;

    for(n = 0; n < _nc; n ++)
    {
        if(0 == strcmp(name, _color_field[n].name))
        {
            return (_color_field[n].val);
        }
    }

    return (_color_field[0].val);
}

GLfloat *Color::get_val_by_index(int n)
{
    if(n < _nc)
        return (_color_field[n].val);
    else
        return (_color_field[0].val);
}

int Color::get_color_index(const char *name)
{
    int n;

    for(n = 0; n < _nc; n++)
    {
        if(0 == strcmp(name, _color_field[n].name))
        {
            return n;
        }
    }

    return 0;
}

