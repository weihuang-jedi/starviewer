#ifndef COLOR_H
#define COLOR_H

/*
 * the colors are from copy and modified from
 * http://cd.textfiles.com/fishandmore/Fish/Graphics/Applications/DKBTrace/DATA/colour.dat
 */

#include <QGLWidget>
#include <QString>

typedef struct _color_field_struct
{
    char name[20];
    GLfloat val[3];
} color_field_struct;

class Color
{
    private:
        int _nc;
        int _max_nc;
        color_field_struct *_color_field;

        void _set_color_field(const char *name, GLfloat r, GLfloat g, GLfloat b);
    public:
        Color();	//  Constructor
        ~Color();	//  Deconstructor

        GLfloat *get_val_by_name(const char *name);
        GLfloat *get_val_by_index(int n);
        void     set_color(const char *name, GLfloat r, GLfloat g, GLfloat b);
        int      get_color_index(const char *name);
};

#endif

