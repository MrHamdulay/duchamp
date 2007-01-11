#ifndef ATROUS_H
#define ATROUS_H

#ifndef PARAM_H
#include <param.hh>
#endif

#include <vector>
using std::vector;
#include <string>
using std::string;

class Filter
{
public:
  // these are all in atrous.cc
  Filter();                         ///< Constructor
  ~Filter();                        ///< Destructor
  void   define(int code);          ///< Define the parameters for a given filter.
  int    getNumScales(long length); ///< Calculate the number of scales possible with a given filter and data size.
  int    getMaxSize(int scale);     ///< Calculate the maximum number of pixels able to be analysed with a filter at a given scale.

  // these are inline functions.
  string getName();                                      ///< Return filter name.
  double coeff(int i);                                   ///< Return ith filter coefficient
  void   setCoeff(int i, double val);                    ///< Set the ith filter coefficient
  int    width();                                        ///< Return the filter width
  int    maxFactor(int dim);                             ///< Return maximum number of scales possible.
  void   setMaxFactor(int dim, int val);                 ///< Set the maximum number of scales.
  double sigmaFactor(int dim, int scale);                ///< Return the sigma correction factor.
  void   setSigmaFactor(int dim, int scale, double val); ///< Set the sigma correction factor.

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

// INLINE definitions for Filter member functions
inline string Filter::getName(){return name;};
inline double Filter::coeff(int i){return filter1D[i];};
inline void   Filter::setCoeff(int i, double val){filter1D[i] = val;};
inline int    Filter::width(){return filter1D.size();};
inline int    Filter::maxFactor(int dim){return maxNumScales[dim-1];};
inline void   Filter::setMaxFactor(int dim, int val){maxNumScales[dim-1] = val;};
inline double Filter::sigmaFactor(int dim, int scale){return (*sigmaFactors[dim-1])[scale];};
inline void   Filter::setSigmaFactor(int dim, int scale, double val){(*sigmaFactors[dim])[scale] = val;};



// The tolerance in the reconstruction.
const float reconTolerance = 0.005;

//////////////////////////////////////////////////////////////////////////////////////

void atrous1DReconstruct(long &size, float *&input, float *&output, Param &par);
void atrous2DReconstruct(long &xdim, long &ydim, float *&input,float *&output, Param &par);
void atrous3DReconstruct(long &xdim, long &ydim, long &zdim, float *&input,float *&output, Param &par);
void atrous2DReconstructOLD(long &xdim, long &ydim, float *input,float *output, Param &par);
void atrous3DReconstructOLD(long &xdim, long &ydim, long &zdim, float *&input,float *&output, Param &par);

void atrousTransform(long &length, int &numScales, float *spectrum, double *coeffs, double *wavelet);
void atrousTransform(long &length, float *spectrum, float *coeffs, float *wavelet);
void atrousTransform2D(long &xdim, long &ydim, int &numScales, float *input, double *coeffs, double *wavelet, Param &par);
void atrousTransform2D(long &xdim, long &ydim, int &numScales, float *input, double *coeffs, double *wavelet);
void atrousTransform3D(long &xdim, long &ydim, long &zdim, int &numScales, float *&input, float *&coeffs, float *&wavelet, Param &par);
void atrousTransform3D(long &xdim, long &ydim, long &zdim, int &numScales, float *input, float *coeffs, float *wavelet);

void baselineSubtract(long numSpec, long specLength, float *originalCube, float *baseline, Param &par);
void getBaseline(long size, float *input, float *baseline, Param &par);
void getBaseline(long size, float *input, float *baseline);

#endif
