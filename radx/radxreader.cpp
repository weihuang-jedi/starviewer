///////////////////////////////////////////////////////////////
//$Id: radxreader.cpp 5323 2015-02-20 17:38:57Z starviewer $
///////////////////////////////////////////////////////////////

#include "radxreader.h"

#define ENTRYMARK	{ cerr << endl; \
                          _increaseIndent(); \
			  cerr << _indent << "enter <" << __PRETTY_FUNCTION__ << "\tfile: " << __FILE__ << "\tline: " << __LINE__ << endl; }

#define EXITMARK	{ cerr << _indent << "leave <" << __PRETTY_FUNCTION__ << "\tfile: " << __FILE__ << "\tline: " << __LINE__ << endl << endl; \
			  _decreaseIndent(); \
			  cerr << endl; }

#define MAKEMARK	cerr << endl << _indent << "file: " << __FILE__ << "\tline: " << __LINE__ << endl << endl;

//Constructor
RadxReader::RadxReader(string flnm)
{
  //cerr << "\nfile: " << __FILE__ << "\tline: " << __LINE__ << endl;

    _indent_length = 0;

  //set programe name
    _progName = __PRETTY_FUNCTION__;

    cerr << "\t_progName: " << _progName << endl;
  
  //parse command line args
    _path = flnm;

    cerr << "\t_path: " << _path << endl;

    _indent = new char[INDETN_LENGTH];

    memset(_indent, 0, INDETN_LENGTH);

  //cerr << "file: " << __FILE__ << "\tline: " << __LINE__ << endl << endl;

  //file = GenericRadxFile();
    file = RadxFile();
}

//destructor
RadxReader::~RadxReader()
{
    delete [] _indent;

  //cerr << "\tfile: " << __FILE__ << "\tline: " << __LINE__ << endl;
}

void RadxReader::_increaseIndent()
{
    _indent[_indent_length] = '\t';
    ++_indent_length;
}

void RadxReader::_decreaseIndent()
{
    --_indent_length;
    _indent[_indent_length] = '\0';
}

int RadxReader::processFile()
{
  //ENTRYMARK;

  //does file exist?
    struct stat fileStat;
    if(stat(_path.c_str(), &fileStat))
    {
        int errNum = errno;
        cerr << "ERROR - RadxReader::_handleViaPath" << endl;
        cerr << "  Cannot stat file: " << _path << endl;
        cerr << strerror(errNum) << endl;
        return -1;
    }

  //MAKEMARK;

  //set up file
  //file.setDebug(true);
  //file.setVerbose(true);

  //MAKEMARK;

  //native print?
  //if(file.printNative(_path, cout,
  //                    _params.print_rays,
  //                    _params.print_data))
  //{
  //    cerr << "ERROR - RadxReader::_handleViaPath" << endl;
  //    cerr << "  Printing file natively: " << _path << endl;
  //    cerr << file.getErrStr() << endl;
  //    return -1;
  //}

  //set up read
    _setupRead();

  //MAKEMARK;

  //read in file
    if(file.readFromPath(_path, vol))
    {
        cerr << "ERROR - RadxReader::_handleViaPath" << endl;
        cerr << "  Printing file: " << _path << endl;
        cerr << file.getErrStr() << endl;
        return -1;
    }

  //set number of gates constant if requested
  //vol.setNGatesConstant();

  //trim to 360s if requested
  //vol.trimSurveillanceSweepsTo360Deg();

  //MAKEMARK;

    fieldNames = vol.getUniqueFieldNameList();

    _numFields = fieldNames.size();
    if(0 < _numFields)
        _fieldName = fieldNames[0];
    else
        _fieldName = "Unknown";

  //checkfile
    _checkVal();

  //EXITMARK;

    return 0;
}

//set up read
void RadxReader::_setupRead()
{
  //ENTRYMARK;

  //file.setReadFixedAngleLimits(_params.read_lower_fixed_angle,
  //                             _params.read_upper_fixed_angle);
  //file.setReadSweepNumLimits(_params.read_lower_sweep_num,
  //                           _params.read_upper_sweep_num);

  //for(int i = 0; i < _params.read_field_names_n; i++)
  //{
  //    cerr << "_params._read_field_names[" << i << "] = " << _params._read_field_names[i] << endl;
  //    file.addReadField(_params._read_field_names[i]);
  //}

    file.setReadAggregateSweeps(false);
    file.setReadIgnoreTransitions(false);
  //file.setReadMetadataOnly(true);
    file.setReadRemoveRaysAllMissing(false);
    file.setReadPreserveSweeps(false);
    file.setReadRemoveLongRange(false);
    file.setReadRemoveShortRange(false);
  //file.setReadMaxRangeKm(_params.max_range_km);
  //file.setChangeLatitudeSignOnRead(true);
  //file.setApplyGeorefsOnRead(true);
  //file.setReadIgnoreTransitions(true);
  //file.setRadarNumOnRead(_params.read_radar_num);
  //file.printReadRequest(cerr);

  //EXITMARK;
}

//////////////////////////////////////////////////
// perform print

void RadxReader::_checkVal()
{
  //ENTRYMARK;

    RadxPlatform platform = vol.getPlatform();
    vector<double> frequencyHz = platform.getFrequencyHz();
    size_t n = 0;

  //vol.loadFieldsFromRays();
  
  //vol.printWithFieldData(cout);
  //vol.printWithRayMetaData(cout);
  //vol.printRaySummary(cout);
  //_printRayTable();

  //vol.print(cout);
  
    cout << "\nVolume info:" << endl;
    cout << "\ttitle:\t"       << vol.getTitle() << endl;
    cout << "\tinstitution:\t" << vol.getInstitution() << endl;
    cout << "\treferences:\t"  << vol.getReferences() << endl;
    cout << "\tsource:\t"      << vol.getSource() << endl;
    cout << "\thistory:\t"     << vol.getHistory() << endl;
    cout << "\tcomment:\t"     << vol.getComment() << endl;
    cout << "\tscanName:\t"    << vol.getScanName() << endl;
    cout << "\tscanId(VCP):\t" << vol.getScanId() << endl;

  //MAKEMARK;

  //_platform.print(out);
    cout << "\nRadxPlatform:" << endl;
    cout << "\tinstrumentName:\t" << platform.getInstrumentName() << endl;
    cout << "\tsiteName:\t"       << platform.getSiteName() << endl;
    cout << "\tinstrumentType: "  << Radx::instrumentTypeToStr(platform.getInstrumentType()) << endl;
    cout << "\tplatformType:\t"   << Radx::platformTypeToStr(platform.getPlatformType()) << endl;
    cout << "\tprimaryAxis:\t"    << Radx::primaryAxisToStr(platform.getPrimaryAxis()) << endl;
    cout << "\tlatitudeDeg:\t"    << platform.getLatitudeDeg() << endl;
    cout << "\tlongitudeDeg:\t"   << platform.getLongitudeDeg() << endl;
    cout << "\taltitudeKm:\t"     << platform.getAltitudeKm() << endl;
    cout << "\tsensorHtAglM:\t"   << platform.getSensorHtAglM() << endl;
   
  //MAKEMARK;

    if(frequencyHz.size() > 0)
    {
        cout << "\tfrequencyHz:";
        for(n = 0; n < frequencyHz.size(); ++n)
          cout << "\t" << frequencyHz[n];

        cout << endl;
    }
 
    if(platform.getInstrumentType() == Radx::INSTRUMENT_TYPE_RADAR)
    {
        cout << "\tradarAntGainDbH:\t"           << platform.getRadarAntennaGainDbH() << endl;
        cout << "\tradarAntGainDbV:\t"           << platform.getRadarAntennaGainDbV() << endl;
        cout << "\tradarBeamWidthDegH:\t"        << platform.getRadarBeamWidthDegH() << endl;
        cout << "\tradarBeamWidthDegV:\t"        << platform.getRadarBeamWidthDegV() << endl;
        cout << "\tradarReceiverBandwidthMhz:\t" << platform.getRadarReceiverBandwidthMhz() << endl;
    }
    else
    {
        cout << "\tlidarConstant:\t"           << platform.getLidarConstant() << endl;
        cout << "\tlidarPulseEnergyJ:\t"       << platform.getLidarPulseEnergyJ() << endl;
        cout << "\tlidarPeakPowerW:\t"         << platform.getLidarPeakPowerW() << endl;
        cout << "\tlidarApertureDiamCm:\t"     << platform.getLidarApertureDiamCm() << endl;
        cout << "\tlidarApertureEfficiency:\t" << platform.getLidarApertureEfficiency() << endl;
        cout << "\tlidarFieldOfViewMrad:\t"    << platform.getLidarFieldOfViewMrad() << endl;
        cout << "\tlidarBeamDivergenceMrad:\t" << platform.getLidarBeamDivergenceMrad() << endl;
    }

  //MAKEMARK;

    cout << "\nRadx Volume Number: " << vol.getVolumeNumber() << endl;
    cout << "\trhiMode?\t"       << vol.checkIsRhi() << endl;
    cout << "\tstartTimeSecs:\t" << RadxTime::strm(vol.getStartTimeSecs()) << endl;
    cout << "\tstartNanoSecs:\t" << vol.getStartNanoSecs() << endl;
    cout << "\tendTimeSecs:\t"   << RadxTime::strm(vol.getEndTimeSecs()) << endl;
    cout << "\tendNanoSecs:\t"   << vol.getEndNanoSecs() << endl;
    cout << "\tRay times in increasing order? " << vol.getRayTimesIncrease() << endl;
    cout << endl;

    cout << "\tNumber of sweeps:\t" << vol.getNSweeps() << endl;
    cout << "\tNumber of rays:\t"   << vol.getNRays() << endl;
    cout << "\tNumber of calibs:\t" << vol.getNRcalibs() << endl;

    cout << "\tNumber fields: " << fieldNames.size() << endl;
    for(size_t n = 0; n < fieldNames.size(); ++n)
        cout << "\t\tfield[" << n << "]: " << fieldNames[n] << endl;

  //MAKEMARK;

    cout << "\tstartRangeKm:\t" << vol.getStartRangeKm() << endl;
    cout << "\tgateSpacingKm:\t" << vol.getGateSpacingKm() << endl;

    cout << "\tNumber of rays:\t" << vol.getNRays() << endl;
    cout << "\tNumber of points:\t" << vol.getNPoints() << endl;
    cout << "\tmaxNGates:\t" << vol.getMaxNGates() << endl;
    cout << "\tnGatesVary:\t" << vol.getNGatesVary() << endl;
  //cout << "\tsize_t getRayNGates(size_t rayNum):\t" << vol.getRayNGates(n) << endl;
  //RadxRangeGeom::print(cout);
  //RadxPacking::printSummary(cout);

  //MAKEMARK;

    cout << "\nSWEEPS AS IN FILE:" << endl;

    vector<RadxSweep *> sweepsAsInFile = vol.getSweepsAsInFile();
    for(n = 0; n < sweepsAsInFile.size(); ++n)
       sweepsAsInFile[n]->print(cout);

  //MAKEMARK;

    cout << "\nSWEEPS AS IN OBJECT" << endl;

    vector<RadxSweep *> sweeps = vol.getSweeps();
    for(n = 0; n < vol.getNSweeps(); ++n)
        sweeps[n]->print(cout);

  //MAKEMARK;

    cout << "\nFields in file (based on size):" << endl;

    vector<RadxField *> fields = vol.getFields();

    for(n = 0; n < fields.size(); ++n)
        fields[n]->print(cout);

  //MAKEMARK;

    cout << "\nFields in file (based on name):" << endl;

    for(size_t ifield = 0; ifield < fieldNames.size(); ++ifield)
    {
        string fieldName = fieldNames[ifield];
        const RadxField *fld = vol.getFieldFromRay(fieldName);
 
        if(NULL != fld)
        {
            cout << "\nNOTE: Field " << ifield << " is from first ray." << endl;
            fld->print(cout);
            cout << endl << endl;
        }
    }

  //MAKEMARK;

    cout << "\nrcalibs:" << endl;

    vector<RadxRcalib *> rcalibs = vol.getRcalibs();
    for(n = 0; n < rcalibs.size(); ++n)
        rcalibs[n]->print(cout);

  //MAKEMARK;

    cout << "\ncfactors:" << endl;

    RadxCfactors* cfactors = vol.getCfactors();
    if(NULL != cfactors)
        cfactors->print(cout);

  //EXITMARK;
}

//////////////////////////////////////////////////
// print out ray table

void RadxReader::_printRayTable()
{
    size_t n = 0;
    const vector<RadxRay *> rays = vol.getRays();

    ENTRYMARK;

    for(n = 0; n < rays.size(); n++)
    {
        const RadxRay &ray = *rays[n];

        RadxTime rtime(ray.getTimeSecs());

        cout << "Ray No " << n << ": "
             << rtime.getYear() << "\t"
             << rtime.getMonth() << "\t"
             << rtime.getDay() << "\t"
             << rtime.getHour() << "\t"
             << rtime.getMin() << "\t"
             << rtime.getSec() << "\t"
             << ray.getNanoSecs() * 1.0e-9 << endl;
        cout << "\t"
             << ray.getElevationDeg() << "\t"
             << ray.getAzimuthDeg() << "\t"
             << ray.getFixedAngleDeg() << "\t"
             << ray.getAntennaTransition() << "\t"
             << ray.getNGates() << "\t"
             << ray.getStartRangeKm() << "\t"
             << ray.getGateSpacingKm() << "\t"
             << ray.getSweepNumber() << endl;
        cout << "\t\t"
             << ray.getPrtSec() << "\t"
             << ray.getNyquistMps() << "\t"
             << ray.getUnambigRangeKm() << "\t"
             << ray.getMeasXmitPowerDbmH() << "\t"
             << ray.getMeasXmitPowerDbmV() << "\t"
             << ray.getEstimatedNoiseDbmHc() << " "
             << ray.getEstimatedNoiseDbmVc() << " "
             << ray.getEstimatedNoiseDbmHx() << " "
             << ray.getEstimatedNoiseDbmVx() << " "
             << ray.getEventFlagsSet() << " "
             << ray.getStartOfSweepFlag() << " "
             << ray.getEndOfSweepFlag() << " "
             << ray.getStartOfVolumeFlag() << " "
             << ray.getEndOfVolumeFlag() << endl;
    }

    EXITMARK;
}

int RadxReader::setup()
{
  //ENTRYMARK;

  //does file exist?
    struct stat fileStat;
    if(stat(_path.c_str(), &fileStat))
    {
        int errNum = errno;
        cerr << "ERROR - RadxReader::_handleViaPath" << endl;
        cerr << "  Cannot stat file: " << _path << endl;
        cerr << strerror(errNum) << endl;
        return -1;
    }

  //MAKEMARK;

  //set up read
    _setupRead();

  //MAKEMARK;

  //read in file
    if(file.readFromPath(_path, vol))
    {
        cerr << "ERROR - RadxReader::_handleViaPath" << endl;
        cerr << "  Printing file: " << _path << endl;
        cerr << file.getErrStr() << endl;
        return -1;
    }

  //set number of gates constant if requested
  //vol.setNGatesConstant();

  //trim to 360s if requested
  //vol.trimSurveillanceSweepsTo360Deg();

  //MAKEMARK;

    fieldNames = vol.getUniqueFieldNameList();

    _numFields = fieldNames.size();
    if(0 < _numFields)
        _fieldName = fieldNames[0];
    else
        _fieldName = "Unknown";

  //checkfile
    RadxPlatform platform = vol.getPlatform();

  //vol.loadFieldsFromRays();
  
  //cout << "\nVolume info:" << endl;
  //cout << "\ttitle:\t"       << vol.getTitle() << endl;
  //cout << "\tinstitution:\t" << vol.getInstitution() << endl;
  //cout << "\treferences:\t"  << vol.getReferences() << endl;
  //cout << "\tsource:\t"      << vol.getSource() << endl;
  //cout << "\thistory:\t"     << vol.getHistory() << endl;
  //cout << "\tcomment:\t"     << vol.getComment() << endl;
  //cout << "\tscanName:\t"    << vol.getScanName() << endl;
  //cout << "\tscanId(VCP):\t" << vol.getScanId() << endl;

  //MAKEMARK;

  //_platform.print(out);
  //cout << "\nRadxPlatform:" << endl;
  //cout << "\tinstrumentName:\t" << platform.getInstrumentName() << endl;
  //cout << "\tsiteName:\t"       << platform.getSiteName() << endl;
  //cout << "\tinstrumentType: "  << Radx::instrumentTypeToStr(platform.getInstrumentType()) << endl;
  //cout << "\tplatformType:\t"   << Radx::platformTypeToStr(platform.getPlatformType()) << endl;
  //cout << "\tprimaryAxis:\t"    << Radx::primaryAxisToStr(platform.getPrimaryAxis()) << endl;
  //cout << "\tlatitudeDeg:\t"    << platform.getLatitudeDeg() << endl;
  //cout << "\tlongitudeDeg:\t"   << platform.getLongitudeDeg() << endl;
  //cout << "\taltitudeKm:\t"     << platform.getAltitudeKm() << endl;
  //cout << "\tsensorHtAglM:\t"   << platform.getSensorHtAglM() << endl;
   
  //MAKEMARK;

  //if(platform.getInstrumentType() == Radx::INSTRUMENT_TYPE_RADAR)
  //{
  //    cout << "\tradarAntGainDbH:\t"           << platform.getRadarAntennaGainDbH() << endl;
  //    cout << "\tradarAntGainDbV:\t"           << platform.getRadarAntennaGainDbV() << endl;
  //    cout << "\tradarBeamWidthDegH:\t"        << platform.getRadarBeamWidthDegH() << endl;
  //    cout << "\tradarBeamWidthDegV:\t"        << platform.getRadarBeamWidthDegV() << endl;
  //    cout << "\tradarReceiverBandwidthMhz:\t" << platform.getRadarReceiverBandwidthMhz() << endl;
  //}
  //else
  //{
  //    cout << "\tlidarConstant:\t"           << platform.getLidarConstant() << endl;
  //    cout << "\tlidarPulseEnergyJ:\t"       << platform.getLidarPulseEnergyJ() << endl;
  //    cout << "\tlidarPeakPowerW:\t"         << platform.getLidarPeakPowerW() << endl;
  //    cout << "\tlidarApertureDiamCm:\t"     << platform.getLidarApertureDiamCm() << endl;
  //    cout << "\tlidarApertureEfficiency:\t" << platform.getLidarApertureEfficiency() << endl;
  //    cout << "\tlidarFieldOfViewMrad:\t"    << platform.getLidarFieldOfViewMrad() << endl;
  //    cout << "\tlidarBeamDivergenceMrad:\t" << platform.getLidarBeamDivergenceMrad() << endl;
  //}

  //MAKEMARK;

  //cout << "\nRadx Volume Number: " << vol.getVolumeNumber() << endl;
  //cout << "\trhiMode?\t"       << vol.checkIsRhi() << endl;
  //cout << "\tstartTimeSecs:\t" << RadxTime::strm(vol.getStartTimeSecs()) << endl;
  //cout << "\tstartNanoSecs:\t" << vol.getStartNanoSecs() << endl;
  //cout << "\tendTimeSecs:\t"   << RadxTime::strm(vol.getEndTimeSecs()) << endl;
  //cout << "\tendNanoSecs:\t"   << vol.getEndNanoSecs() << endl;
  //cout << "\tRay times in increasing order? " << vol.getRayTimesIncrease() << endl;
  //cout << endl;

    cout << "\tNumber of sweeps:\t" << vol.getNSweeps() << endl;
    cout << "\tNumber of rays:\t"   << vol.getNRays() << endl;
  //cout << "\tNumber of calibs:\t" << vol.getNRcalibs() << endl;

  //cout << "\tNumber fields: " << fieldNames.size() << endl;
  //for(size_t n = 0; n < fieldNames.size(); ++n)
  //    cout << "\t\tfield[" << n << "]: " << fieldNames[n] << endl;

  //MAKEMARK;

  //cout << "\tstartRangeKm:\t" << vol.getStartRangeKm() << endl;
    cout << "\tgateSpacingKm:\t" << vol.getGateSpacingKm() << endl;

  //cout << "\tNumber of rays:\t" << vol.getNRays() << endl;
  //cout << "\tNumber of points:\t" << vol.getNPoints() << endl;
  //cout << "\tmaxNGates:\t" << vol.getMaxNGates() << endl;
  //cout << "\tnGatesVary:\t" << vol.getNGatesVary() << endl;
  //cout << "\tsize_t getRayNGates(size_t rayNum):\t" << vol.getRayNGates(n) << endl;
  //RadxRangeGeom::print(cout);
  //RadxPacking::printSummary(cout);

    _siteName = platform.getInstrumentName();
  //_siteName = platform.getSiteName();
    _baseLat  = platform.getLatitudeDeg();
    _baseLon  = platform.getLongitudeDeg();
    _baseAlt  = platform.getAltitudeKm();

    _startTime = vol.getStartTimeSecs() + vol.getStartNanoSecs() * 1.0e-9;
    _numFields = (int)fieldNames.size();
    _numSweeps = (int)vol.getNSweeps();
    _numRays   = (int)vol.getNRays();
    _numPoints = (int)vol.getNPoints();
    _numBins   = _numPoints;
    _gateSpace = vol.getGateSpacingKm();

  //EXITMARK;

    return 0;
}

RadxVol* RadxReader::getRadxVol(string vn)
{
    _fieldName = vn;
    return &vol;
}

