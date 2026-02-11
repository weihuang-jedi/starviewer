/////////////////////////////////////////////////////////////
//$Id: radxreader.h 5321 2015-02-20 03:20:52Z starviewer $
///////////////////////////////////////////////////////////////

#ifndef _RadxReader_H
#define _RadxReader_H

#include <iostream>
#include <cerrno>
#include <string>
#include <sys/stat.h>

#include <string.h>

#include <Radx/RadxRay.hh>
#include <Radx/RadxSweep.hh>
#include <Radx/RadxField.hh>
#include <Radx/RadxRcalib.hh>
#include <Radx/RadxCfactors.hh>
#include <Radx/RadxVol.hh>
//#include <Radx/DoradeRadxFile.hh>
#include <Radx/RadxTime.hh>
#include <Radx/RadxTimeList.hh>
#include <NcfMdv/GenericRadxFile.hh>

using namespace std;

#define	INDETN_LENGTH	1024

class RadxReader
{
    public:
        RadxReader(string flnm);	//constructor
       ~RadxReader();			//destructor

        int setup();
        int processFile();

        RadxVol* getRadxVol() { return &vol; };
        RadxVol* getRadxVol(string vn);

        void set_varname(string v) { _fieldName = v; };

        int get_nbins() { return _numBins; };
        int get_nrays() { return _numRays; };
        int get_nsweeps() { return _numSweeps; };
        int get_varnumber() { return _numFields; };
        string get_varname() { return _fieldName; };
        vector<string> get_varnames() { return fieldNames; };

        string get_siteName() { return _siteName; };
        double get_baseLat() { return _baseLat; };
        double get_baseLon() { return _baseLon; };
        double get_baseAlt() { return _baseAlt; };
        double get_startTime() { return _startTime; };
        double get_gateSpace() { return _gateSpace; };

    protected:
      //GenericRadxFile file;
        RadxFile file;
        RadxVol vol;

        vector<string> fieldNames;

    private:
        string _progName;
        string _path;
        string _fieldName;
        string _siteName;

        int _numFields;
        int _numSweeps;
        int _numRays;
        int _numPoints;
        int _numBins;

        char* _indent;
        int _indent_length;

        double _baseLat;
        double _baseLon;
        double _baseAlt;
        double _startTime;
        double _gateSpace;

        void _setupRead();
        void _checkVal();
        void _printRayTable();

        void _increaseIndent();
        void _decreaseIndent();
};
#endif

