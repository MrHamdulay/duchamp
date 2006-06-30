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
