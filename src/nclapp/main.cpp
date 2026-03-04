#include <stdio.h>

#include "nclfilereader.h"

int main(int argc, char *argv[])
{
    int n;
    string filename;
    char *path = NULL;
    NclFileReader *filereader;
    float *val;

    fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
    fprintf(stderr, "\targc = %d\n", argc);
    for(n = 0; n < argc; ++n)
    {
       fprintf(stderr, "\targv[%d]: <%s>\n", n, argv[n]);
       if(1 == n)
       {
          path = argv[1];
          filename = string(path);
       }
    }

    if(NULL == path)
    {
       return (-1);
    }

    fprintf(stderr, "\nfile: %s, line: %d\n", __FILE__, __LINE__);
    fprintf(stderr, "\tpath: <%s>\n", path);
    filereader = new NclFileReader(filename);

    filereader->print();

  //val = filereader->get_fv("QFX");

  //fprintf(stderr, "\tval[0] = %f\n", val[0]);

    delete filereader;

    return (0);
}

