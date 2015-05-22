/*=============================================================================
//	
//  This software has been released under the terms of the GNU General Public
//  license. See http://www.gnu.org/copyleft/gpl.html for details.
//
//  Copyright 2002 Anders Johansson ajh@atri.curtin.edu.au
//
//=============================================================================
*/

/* This file contains the resampling engine, the sample format is
   controlled by the FORMAT parameter, the filter length by the L
   parameter and the resampling type by UP and DN. This file should
   only be included by af_resample.c 
*/ 

#undef L
#undef SHIFT
#undef FORMAT
#undef FIR
#undef ADDQUE

/* The length Lxx definition selects the length of each poly phase
   component. Valid definitions are L8 and L16 where the number
   defines the nuber of taps. This definition affects the
   computational complexity, the performance and the memory usage.
*/

/* The FORMAT_x parameter selects the sample format type currently
   float and int16 are supported. Thes two formats are selected by
   defining eiter FORMAT_F or FORMAT_I. The advantage of using float
   is that the amplitude and therefore the SNR isn't affected by the
   filtering, the disadvantage is that it is a lot slower.
*/


#define FORMAT int16_t



typedef struct af_resample_s
{
   void*  	w;	// Current filter weights
   void** 	xq; 	// Circular buffers
   uint32_t	xi; 	// Index for circular buffers
   uint32_t	wi;	// Index for w
   uint32_t	i; 	// Number of new samples to put in x queue 
   uint32_t  	dn;     // Down sampling factor
   uint32_t	up;	// Up sampling factor 
   uint32_t	step;	// Step size for linear interpolation
   uint32_t	pt;	// Pointer remainder for linear interpolation
   int		setup;	// Setup parameters cmdline or through postcreate
   int      init;
   short    in16tmp[8];
} af_resample_t;

#define L8

#ifdef L8
#define L   	8
#else
#define L   	16
#endif



