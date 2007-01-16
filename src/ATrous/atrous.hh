#ifndef ATROUS_H
#define ATROUS_H

#ifndef PARAM_H
#include <param.hh>
#endif

#include <vector>
using std::vector;
#include <string>
using std::string;

/**
 * A class to store details of the wavelet's filter.
 * This stores all details of and functions related to the filter used to
 *  generate the wavelets used in the reconstruction (and baseline
 *  subtraction) functions.
 * 
 * It stores the coefficients of the filter, as well as the number of scales
 * associated with it, and the sigma factors, that relate the value of the rms
 * at a given scale with the measured value.
 */

class Filter
{
public:
  // these are all in atrous.cc
  Filter();                         ///< Constructor
  ~Filter();                        ///< Destructor

  /** Define the parameters for a given filter. */
  void   define(int code);

  /** Calculate the number of scales possible with a given filter and data size. */
  int    getNumScales(long length);

  /** Calculate the maximum number of pixels able to be analysed with a filter at a given scale. */
  int    getMaxSize(int scale);

  /** Return the width of the filter */
  int    width(){return filter1D.size();};

  // these are inline functions.
  string getName(){return name;};
  double coeff(int i){return filter1D[i];};
  void   setCoeff(int i, double val){filter1D[i] = val;};
  int    maxFactor(int dim){return maxNumScales[dim-1];};
  void   setMaxFactor(int dim, int val){maxNumScales[dim-1] = val;};
  double sigmaFactor(int dim, int scale){return (*sigmaFactors[dim-1])[scale];};
  void   setSigmaFactor(int dim, int scale, double val){(*sigmaFactors[dim])[scale] = val;};

private:
  string name;                ///< what is the filter called?
  vector <double> filter1D;   ///< filter coefficients.
  vector <int> maxNumScales;  ///< max number of scales for the sigmaFactor arrays, for each dim.
  vector < vector <double>* > sigmaFactors; ///< arrays of sigmaFactors, one for each dim.

  // these are all in atrous.cc
  void   loadSpline();        ///< set up parameters for using the B3 Spline filter.
  void   loadTriangle();      ///< set up parameters for using the Triangle function.
  void   loadHaar();          ///< set up parameters for using the Haar wavelet.

};


const float reconTolerance = 0.005; ///< The tolerance in the reconstruction.


//////////////////////////////////////////////////////////////////////////////////////

/** Perform a 1-dimensional a trous wavelet reconstruction. */
void atrous1DReconstruct(long &size, float *&input, float *&output, Param &par);

/** Perform a 2-dimensional a trous wavelet reconstruction. */
void atrous2DReconstruct(long &xdim, long &ydim, float *&input,float *&output, Param &par);

/** Perform a 3-dimensional a trous wavelet reconstruction. */
void atrous3DReconstruct(long &xdim, long &ydim, long &zdim, float *&input,float *&output, Param &par);

/** Subtract a baseline from a set of spectra in a cube. */
void baselineSubtract(long numSpec, long specLength, float *originalCube, float *baseline, Param &par);

/** Find the baseline of a 1-D spectrum. */
void getBaseline(long size, float *input, float *baseline, Param &par);

/** Find the baseline of a 1-D spectrum. */
void getBaseline(long size, float *input, float *baseline);

#endif
