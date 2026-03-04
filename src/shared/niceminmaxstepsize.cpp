//$Id: niceminmaxstepsize.cpp 5314 2015-02-06 20:58:52Z starviewer $

#include <iostream>

#include <float.h>
#include <math.h>

#include "niceminmaxstepsize.h"

using namespace std;

NiceMinMaxStepSize::NiceMinMaxStepSize()
{
}

NiceMinMaxStepSize::~NiceMinMaxStepSize()
{
}

void NiceMinMaxStepSize::process(double min, double max, int max_steps, bool outside)
{
    double table[15] = { 1.0,  2.0,  2.5,  4.0,  5.0,
                     10.0, 20.0, 25.0, 40.0, 50.0,
                     100.0,200.0,250.0,400.0,500.0 };

    double d,u,t,am1,ax1;
    int    npts = 15;
    int    i;

    min_dbl=0.0;
    max_dbl=0.0;

    if(cmpAny2(max, min, 7, MIN_NONZERO) <= 0.0)
    {
        cerr << "Max value less than or equal to min value in " << __PRETTY_FUNCTION__ << endl;
        return;
    }
        
    d = pow(10.0,floor(log10(max - min)) - 2.0);
    step_dbl = FLT_MAX;
    u = 0.5 * step_dbl;
    if(outside)
    {
        for(i = 0; i < npts; ++i)
        {
            t = table[i] * d;
            am1 = floor(min/t) * t;
            ax1 = ceil(max/t) * t;

            if(((i >= (npts-1))&&(step_dbl > u))||
               ((t <= step_dbl)&&
                (cmpAny2(((ax1-am1)/t), (double)(max_steps - 1),7, MIN_NONZERO) <= 0.0)))
            {
                step_dbl = t;
                max_dbl = ax1;
                min_dbl = am1;
            }
        }
    }
    else
    {
        for(i=0;i<npts; i++)
        {
            t = table[i] * d;
            am1 = ceil(min/t) * t;
            ax1 = floor(max/t) * t;
            if(((i>=npts-1)&&(step_dbl > u))||
               ((t <= step_dbl)&&
                (cmpAny2(((ax1-am1)/t), (double)(max_steps - 1),7, MIN_NONZERO) <= 0.0)))
            {
                step_dbl = t;
                max_dbl = ax1;
                min_dbl = am1;
            }
        }
    }

    min_flt = (float) min_dbl;
    max_flt = (float) max_dbl;
    step_flt = (float) step_dbl;
}

void NiceMinMaxStepSize::process(float min, float max, int max_steps, bool outside)
{
    double dmin = (double) min;
    double dmax = (double) max;

    process(dmin, dmax, max_steps, outside);
  //process(dmax, dmin, max_steps, outside);
}

double NiceMinMaxStepSize::cmpAny2(double a, double b, int sig_dig, double min_nonzero)
{
    double afr,bfr,minfr;
    int    aexp,bexp,minexp;
    long   arnd,brnd;
    int    asign,bsign;
    int    diffexp;
    int    aexp10,bexp10,diffexp10;
    int    icount = 0;
    bool   azero,bzero;
    double factor;
    double norm_factor,anorm,bnorm;

#if CMPF_DEBUG
    cerr << "comparing " << a << " and " << b << " to " << sig_dig << endl;
#endif

    if(sig_dig > 7) 
        sig_dig = 7;

    afr = frexp(a, &aexp);
    bfr = frexp(b, &bexp);

    minfr = fabs(frexp(min_nonzero,&minexp));
    asign = afr < 0.0 ? -1 : 1;
    bsign = bfr < 0.0 ? -1 : 1;

#if CMPF_DEBUG
    cerr << "base 2 decomposition a: " << a << " " << aexp;
    cerr << " and b: " << b << " " << bexp << endl;
#endif

    azero = afr == 0.0 || aexp < minexp ||
           (aexp == minexp && afr * asign < minfr);
    bzero = bfr == 0.0 || bexp < minexp ||
           (bexp == minexp && bfr * bsign < minfr);

    if(azero && bzero)
    {
#if CMPF_DEBUG
        cerr << "numbers smaller than minimum non-zero value: " << min_nonzero << endl;
#endif
        return 0.0;
    }

    if (azero)
        return (float) - bsign;

    if (bzero)
        return (float) asign;
    
    diffexp = fabs(aexp - bexp);
    if(diffexp >= 4)
    {
      /*
       *difference exceeds range of single digit decimal precision
       */
#if CMPF_DEBUG
        cerr << "difference exceeds factor of 10e1" << endl;
#endif

        switch (asign)
        {
            case 1:
                 switch (bsign)
                 {
                     case 1:
                          return (float)aexp - bexp;
                     case -1:
                          return 1.0;
                 }
            case -1:
                  switch (bsign)
                  {
                      case 1:
                          return -1.0;
                      case -1:
                          return (float)bexp - aexp;
                  }
         }
    }

    aexp10 = EXP2TOEXP10 * aexp;

#if CMPF_DEBUG
    cerr << "a base 2 exp: " << aexp << " a base 10 exp: " << aexp10 << endl;
#endif

/*
 * normalize the original values such that the greatest absolute value is
 * in the range .1 <= x < 1.0. Two step process: the first step seems to get
 * within 1 digit either way, so perhaps the "while" could be replaced with
 * and "if", but this is not proven.
 */
    norm_factor = pow(10.0,-aexp10); 
    anorm = a * norm_factor;
    while (anorm * asign >= 1.0)
    {
        ++icount;
        ++aexp10;
        anorm *= 0.1;
    }

    while (anorm * asign < 0.1)
    {
        ++icount;
        --aexp10;
        anorm *= 10.0;
    }

    bexp10 = EXP2TOEXP10 * bexp;

#if CMPF_DEBUG
    cerr << "a normalized after " << icount << " iterations: ";
    cerr << anorm << " " << aexp10 << endl;
    cerr << "a base 2 exp: " << aexp << " a base 10 exp: " << aexp10 << endl;
#endif

    norm_factor = pow(10.0,-bexp10); 
    bnorm = b * norm_factor;
    while (bnorm * bsign >= 1.0)
    {
        ++icount;
        ++bexp10;
        bnorm *= .1;
    }

    while (bnorm * bsign < 0.1)
    {
        ++icount;
        --bexp10;
        bnorm *= 10;
    }

#if CMPF_DEBUG
    cerr << "b normalized after " << icount << " iterations: ";
    cerr << bnorm << " " << bexp10 << endl;
    cerr << "b base 2 exp: " << bexp << " b base 10 exp: " << bexp10 << endl;
#endif

/*
 * Convert to longs in the range 10e(sig_digits) <= x < 10e(sig_digits+1)
 * rounded in the units digit.
 */

    factor = pow(10.0,(double)sig_dig);
    arnd = anorm * factor + (asign == 1 ? .5 : -.5);
    brnd = bnorm * factor + (bsign == 1 ? .5 : -.5);

/*
 * ASSERT: 
 * exponents never differ by more than 2
 * (compare .1 and .000999999999 noting that .1/(2^4) = 0.00625) 
 * This allows a switch rather than a while loop here.
 */         
    diffexp10 = aexp10 - bexp10;

    switch (diffexp10)
    {
        case -2:
             brnd *= 100;
             break;
        case -1:
             brnd *= 10;
             break;
        case 0:
             break;
        case 1:
             arnd *= 10;
             break;
        case 2:
             arnd *= 100;
             break;
        default:
             cerr << "Algorithmic error in " << __PRETTY_FUNCTION__ << endl;
             break;
    }
    
#if CMPF_DEBUG
    cerr << "rounded values converted to longs in range 10e(sig_digits): ";
    cerr << arnd << " " << brnd << endl;
#endif

    return (double) (arnd - brnd);
}

