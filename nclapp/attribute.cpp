#include "attribute.h"

Attribute::Attribute()
{
    name = " ";
    value = " ";
}

Attribute::Attribute(string n)
{
    name = n;
    value = " ";
}

Attribute::Attribute(string n, string v)
{
    name = n;
    value = v;
}

Attribute::~Attribute()
{
}

void Attribute::set_name(const char* nc)
{
    name = nc;
}

void Attribute::set_value(const char* vc)
{
    value = vc;
}

void Attribute::set_value(short s)
{
    stringstream ss;
    ss << s;
    value = ss.str();
}

void Attribute::set_value(int i)
{
    stringstream ss;
    ss << i;
    value = ss.str();
}

void Attribute::set_value(float f)
{
    stringstream ss;
    ss << f;
    value = ss.str();
}

void Attribute::set_value(double d)
{
    stringstream ss;
    ss << d;
    value = ss.str();
}

