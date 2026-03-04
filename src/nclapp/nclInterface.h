#ifndef _nclInterface_H
#define _nclInterface_H

#include <ncarg/c.h>
#include <ncarg/hlu/hluP.h>
#include <ncarg/hlu/Error.h>
#include <ncarg/hlu/NresDB.h>
#include <ncarg/hlu/PlotManager.h>
#include <ncarg/hlu/Workstation.h>
#include <ncarg/hlu/Workspace.h>
#include <ncarg/hlu/Callbacks.h>
#include <ncarg/ncargC.h>
#include <ncarg/c.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

#include <cairo/cairo.h>

#include <ncarg/ncl/defs.h>
#include <ncarg/ncl/NclMultiDValData.h>
#include <ncarg/ncl/NclDataDefs.h>
#include <ncarg/ncl/HluClasses.h>
#include <ncarg/ncl/ApiRecords.h>
#include <ncarg/ncl/NclApi.h>
#include <ncarg/ncl/NclFile.h>
#include <ncarg/ncl/NclAdvancedFile.h>
#include <ncarg/ncl/NclVar.h>
#include <ncarg/ncl/NclList.h>
#include <ncarg/ncl/NclFileInterfaces.h>
#include <ncarg/ncl/NclAdvancedFileStructure.h>

#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>
#include <ncarg/hlu/Workstation.h>
#include <ncarg/hlu/ConvertP.h>
#include <ncarg/hlu/Error.h>
#include <ncarg/hlu/App.h>

#include <ncarg/gks.h>
#include <ncarg/ncargC.h>

//Include a header file for each object created
#include <ncarg/hlu/NcgmWorkstation.h>
#include <ncarg/hlu/PSWorkstation.h>
#include <ncarg/hlu/PDFWorkstation.h>
#include <ncarg/hlu/CairoWorkstation.h>
#include <ncarg/hlu/ScalarField.h>
#include <ncarg/hlu/ContourPlot.h>

extern "C"
{
#if 1
    NclFile NclCreateAdvancedFile(const char *path);
#else
    NclAdvancedFile NclCreateAdvancedFile(const char *path);
#endif
    NclFile NclCreateFile(const char *path);
    NclVar  readNclFileVar(NclFile thefile, const char *var_name, NclSelectionRecord *sel_ptr);
    char**   GetNclFileVarNames(NclFile thefile, int *num_vars);
    void     getNclFileVarInfo(NclFile thefile, int *ndims, int **dimsizes, char ***dimnames, long *type);
    char*    NclQuarkToString(NclQuark quark);
    NclQuark NclStringToQuark(const char *str);
    void guiSetAdvancedFileStructure(const char *format);

    void initializeNcl();
    void finalizeNcl();
    void deleteNclObj(NclObj tmp);

    float*  guiGetValue(NclVar _nclvar);
    int*    guiGetIntArray(NclVar _nclvar);
    short*  guiGetShortArray(NclVar _nclvar);
    double* guiGetDoubleArray(NclVar _nclvar);
    char*   guiGetCharArray(NclVar _nclvar);
    double** guiGetListArray(NclVar _nclvar, int** itemsizes);

    void guiNhlInitialize();
    void guiNhlCreate(int *plotid, const char *name,
                      NhlClass nhlclass, int pid, int rlid);
    void guiNhlDraw(int id);
    void guiNhlFrame(int fid);
    void guiNhlDestroy(int id);
    void guiNhlClose();

    void guiNhlSetValues(int plotid, int rlid);

    void guiNhlGetValues(int plotid, int rlid);

    int  guiNhlRLCreate(NhlRLType type);
    void guiNhlRLClear(int id);

    void guiNhlRLSetFloat(int id, const char *resname, float value);
    void guiNhlRLSetInteger(int id, const char *resname, int value);
    void guiNhlRLSetString(int id, const char *resname, const char *value);
    void guiNhlRLSetMDFloatArray(int id, const char *resname,
                                 float *data, int num_dimensions,
                                 int *len_dimensions);
    void guiNhlRLSetFloatArray(int id, const char *resname, float *data,
                               int num_elements);
    void guiNhlRLSetStringArray(int id, char *resname, char **data, int nelem);
    void guiNhlSetColor(int pid, int ci, float red, float green, float blue);

    void guiNhlRLGetFloatArray(int id, const char *name,
                                 float **data, int *num_elements);
    void guiNhlRLGetFloat(int id, const char *resname, float *data);
    void guiNhlRLGetIntegerArray(int id, const char *name,
                                 int **data, int *ne);

    void setCairoQtSurface(cairo_surface_t *surface);
    void setCairoQtWinSize(int width, int height);
    cairo_t *getCairoQtContext();

    int guiGetBoundary(int pid, float *left, float *rite,
                       float *bot, float *top);

    char*  guiQuarkToString(NclQuark qn);
    char** guiGetNclFileAttNames(NclFile thefile, int *num_atts);

    void guiDestroyObj(NclObj obj);
    NclMultiDValData guiGetFileAtt(NclFile thefile, const char *attname);
    NclMultiDValData guiGetVarAtt(NclVar the_var, const char *attname);
    NclMultiDValData guiGetVarAttMV(NclFile thefile, const char* varname, const char* attname);

    void guiNhlAddOverlay(int base_id, int transform_id, int after_id);
    void guiGetEndpointsAndStepSize(float min_in, float max_in,
                                    int max_steps, int outside,
                                    float *min_out, float *max_out,
                                    float *step_size);
}
#endif

