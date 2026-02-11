#include "dimension.h"

Dimension::Dimension()
{
    name = "unknown";
    size = 0;
}

Dimension::Dimension(string sn)
{
    name = sn;
    size = 0;
}

Dimension::Dimension(string sn, int iv)
{
    name = sn;
    size = iv;
}

Dimension::~Dimension()
{
}

