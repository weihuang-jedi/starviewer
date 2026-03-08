#ifndef NICE_MIN_MAX_STEP_SIZE_H
#define NICE_MIN_MAX_STEP_SIZE_H

//This code is based on	<nicevals.c> from NCL.
/************************************************************************
*									*
*			     Copyright (C)  2014			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/

#define CMPF_DEBUG	0
#define	MIN_NONZERO	(1.0e-32)
#define	EXP2TOEXP10	(0.3010299956639812)

/*
 * Function:	_NhlGetEndpointsAndStepSize
 *
 * Description: Given min and max values of a data domain and the maximum
 *              number of steps desired, determines "nice" values of 
 *              for endpoints and spacing to create a series of steps 
 *              through the data domain. A flag controls whether the max 
 *              and min are inside or outside the data range.
 *
 * In Args: double min 		the minimum value of the domain
 *          double max   	the maximum value of the domain
 *          int    max_steps	the maximum number of steps desired
 *          bool   outside      controls whether return min/max fall just
 *                              outside or just inside the data domain.
 *				if outside: 
 * 				    *min_out <= min < *min_out + *step_size
 *                                  *max_out >= max > *max_out - *step_size
 *                              if inside:
 * 				    *min_out >= min > *min_out - *step_size
 *                                  *max_out <= max < *max_out + *step_size
 *
 *
 * Out Args: double *min_out     a "nice" minimum value
 *           double *max_out     a "nice" maximum value
 *           double *step_size   a step value such that 
 *                              (where n is an integer < max_steps):
 *                              *min_out + n * *step_size == *max_out 
 *                               with no remainder 
 *
 * Return Values:
 *   	                NhlFATAL if min is larger than max or if they are
 *                      equal within 7 digits of precision. Values less than
 *                      MIN_NONZERO (defined in hluutil.h) are considered
 *                      equal to 0.0.
 *
 */

/*
 *code is based on <cmpf.c> from NCL.
 *
 * Function:	double cmpAny2(double a, double b, int sig_dig, double min_nonzero)
 *
 * Description: A new parameter allows specification of a minimum nonzero value.
 *              Also: Now uses frexp to break numbers up without fear of
 *              overflow. Numbers are normalized to the range 
 *              .1 <= fabs(x) < 1.0, using the base 2 exponent to nearly
 *              figure out the base 10 exponent to factor out. Iteration
 *              is then used to get the exact base 10 exponent.
 *		
 *
 * In Args:	a	first floating point number
 *		b	second floating point number
 *		sig_dig	<=7 represents number of significant digits to compare.
 *              min_nonzero the smallest abs value to be treated as nonzero.
 *
 * Out Args:	NONE
 *
 * Return Values: 0 if equal, <0 if a<b, and >0 if a>b
 *
 */

class NiceMinMaxStepSize
{
    public:
        NiceMinMaxStepSize();
       ~NiceMinMaxStepSize();

        void process(double min, double max, int max_steps, bool outside);
        double get_min_dbl() { return min_dbl; };
        double get_max_dbl() { return max_dbl; };
        double get_step_dbl() { return step_dbl; };

        void process(float min, float max, int max_steps, bool outside);
        float get_min_flt() { return min_flt; };
        float get_max_flt() { return max_flt; };
        float get_step_flt() { return step_flt; };

    private:
	double min_dbl;
	double max_dbl;
	double step_dbl;
        double cmpAny2(double a, double b, int sig_dig, double min_nonzero);

	float min_flt;
	float max_flt;
	float step_flt;
};
#endif // NICE_MIN_MAX_STEP_SIZE_H
