#ifndef ATROUS_H
#define ATROUS_H

class Param;

const float reconTolerance = 0.005; ///< The tolerance in the reconstruction.


/////////////////////////////////////////////////////////////////////////

/** Perform a 1-dimensional a trous wavelet reconstruction. */
void atrous1DReconstruct(long &size, float *&input, 
			 float *&output, Param &par);

/** Perform a 2-dimensional a trous wavelet reconstruction. */
void atrous2DReconstruct(long &xdim, long &ydim, float *&input,
			 float *&output, Param &par);

/** Perform a 3-dimensional a trous wavelet reconstruction. */
void atrous3DReconstruct(long &xdim, long &ydim, long &zdim, 
			 float *&input,float *&output, Param &par);

/** Subtract a baseline from a set of spectra in a cube. */
void baselineSubtract(long numSpec, long specLength, 
		      float *originalCube, float *baseline, Param &par);

/** Find the baseline of a 1-D spectrum. */
void getBaseline(long size, float *input, float *baseline, Param &par);

/** Find the baseline of a 1-D spectrum. */
void getBaseline(long size, float *input, float *baseline);

#endif
