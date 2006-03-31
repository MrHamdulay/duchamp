#ifndef ATROUS_H
#define ATROUS_H

#ifndef PARAM_H
#include <param.hh>
#endif

#include <vector>
using std::vector;
#include <string>
using std::string;

// namespace FilterSet
// {
//   const int maxDim = 3;

class Filter
{
public:
  Filter();
  ~Filter(){};
  void   define(int code);
  int    getNumScales(long length);
  int    getMaxSize(int scale);
  string getName(){return name;};
  double coeff(int i){return filter1D[i];};
  void   setCoeff(int i, double val){filter1D[i] = val;};
  int    width(){return filter1D.size();};
  int    maxFactor(int dim){return maxNumScales[dim-1];};
  void   setMaxFactor(int dim, int val){maxNumScales[dim-1] = val;};
  double sigmaFactor(int dim, int scale){return (*sigmaFactors[dim-1])[scale];};
  void   setSigmaFactor(int dim, int scale, double val){(*sigmaFactors[dim])[scale] = val;};

private:
  string name;                // what is the filter called?
  vector <double> filter1D;   // filter coefficients.
  vector <int> maxNumScales;  // max number of scales for the sigmaFactor arrays, for each dim.
  vector < vector <double>* > sigmaFactors; // arrays of sigmaFactors, one for each dim.

  void   loadSpline();        // set up parameters for using the B3 Spline filter.
  void   loadTriangle();      // set up parameters for using the Triangle function.
  void   loadHaar();          // set up parameters for using the Haar wavelet.

};

  //Filter basefilter;

// }

//------------------------------


// Filter used for a trous reconstruction 
//   -- just defined in 1-dim, can expand it to 2- or 3-dim in code.
/* B3spline function */
// const int filterwidth = 5;
// const double filter1D[filterwidth] = {1./16., 1./4., 3./8., 1./4., 1./16.};
/* Triangle function */
// const int filterwidth = 3;
// const double filter1D[filterwidth] = {1./4., 1./2., 1./4.};
/* Haar wavelet */
//       const int filterwidth = 3;
//       const double filter[filterwidth] = {0, 1./2., 1./2.};
/* TopHat wavelet */
//       const int filterwidth = 3;
//       const double filter[filterwidth] = {1./3., 1./3., 1./3.};

// The tolerance in the reconstruction.
const float reconTolerance = 0.005;

// The conversion factors for measured sigma in wavelet arrays to "real" sigma values.
const int maxNumScales3D = 7;
const double sigmaFactors3D[maxNumScales3D+1] = {1.00000000000,9.56543592e-1,1.20336499e-1,3.49500154e-2,
						 1.18164242e-2,4.13233507e-3,1.45703714e-3,5.14791120e-4};

const int maxNumScales2D = 11;
const double sigmaFactors2D[maxNumScales2D+1] = {1.00000000000,8.90796310e-1,2.00663851e-1,8.55075048e-2,
						 4.12474444e-2,2.04249666e-2,1.01897592e-2,5.09204670e-3,
						 2.54566946e-3,1.27279050e-3,6.36389722e-4,3.18194170e-4};
const int maxNumScales1D = 18;
const double sigmaFactors1D[maxNumScales1D+1] = {1.00000000000,7.23489806e-1,2.85450405e-1,1.77947535e-1,
						 1.22223156e-1,8.58113122e-2,6.05703043e-2,4.28107206e-2,
						 3.02684024e-2,2.14024008e-2,1.51336781e-2,1.07011079e-2,
						 7.56682272e-3,5.35055108e-3,3.78341085e-3,2.67527545e-3,
                                                 1.89170541e-3,1.33763772e-3,9.45852704e-4};

//////////////////////////////////////////////////////////////////////////////////////

// int getNumScales(long length); // in atrous.cc

void atrous1DReconstruct(long &size, float *&input, float *&output, Param &par);
void atrous2DReconstruct(long &xdim, long &ydim, float *&input,float *&output, Param &par);
void atrous3DReconstruct(long &xdim, long &ydim, long &zdim, float *&input,float *&output, Param &par);
void atrous2DReconstructOLD(long &xdim, long &ydim, float *input,float *output, Param &par);
void atrous3DReconstructOLD(long &xdim, long &ydim, long &zdim, float *&input,float *&output, Param &par);

// template <class T> void atrousTransform(long &length, T *&spectrum, T *&coeffs, T *&wavelet);

void atrousTransform(long &length, int &numScales, float *spectrum, double *coeffs, double *wavelet);
void atrousTransform(long &length, float *spectrum, float *coeffs, float *wavelet);
void atrousTransform2D(long &xdim, long &ydim, int &numScales, float *input, double *coeffs, double *wavelet, Param &par);
void atrousTransform2D(long &xdim, long &ydim, int &numScales, float *input, double *coeffs, double *wavelet);
void atrousTransform3D(long &xdim, long &ydim, long &zdim, int &numScales, float *&input, float *&coeffs, float *&wavelet, Param &par);
void atrousTransform3D(long &xdim, long &ydim, long &zdim, int &numScales, float *input, float *coeffs, float *wavelet);

void baselineSubtract(long numSpec, long specLength, float *originalCube, float *baseline, Param &par);
void getBaseline(long size, float *input, float *baseline, Param &par);

#endif
