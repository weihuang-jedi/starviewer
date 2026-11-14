#ifndef _MPASGRIDREADER_H
#define _MPASGRIDREADER_H
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>

#include <netcdf.h>

#include "ncbasereader.h"
#include "mpasgridreader.h"
 
class MPASGridReader : public NCBaseReader
{
  public:
    // declare constructor and deconstructor methods
    MPASGridReader(const char*);
    virtual ~MPASGridReader();
 
    int get_total_cells() { return _total_cells; };
    int get_n_partitions() { return _n_partitions; };
    int get_max_cells_per_partition() { return _max_cells_per_partition; };

    void exploreFile() override;
    void get_dim_info() override;
    void get_var_info() override;

  protected:
    int _total_cells;
    int _n_partitions;
    int _max_cells_per_partition;

    // Inside your Viewer/Reader class definition or constructor:
    unordered_map<std::string, int*> _dimMap;
};
#endif
