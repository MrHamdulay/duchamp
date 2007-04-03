#ifndef UTILS_H
#define UTILS_H

#include <wcs.h>
#include <string>
#include <param.hh>

// define the speed of light for WCS-related accessor functions 
const float C_kms = 299792.458;

int linear_regression(int num, float *x, float *y, int ilow, int ihigh, 
		      float &slope, float &errSlope, 
		      float &intercept, float &errIntercept, float &r);
void zscale(long imagesize, float *image, float &z1, float &z2);
void zscale(long imagesize, float *image, float &z1, float &z2, float nullVal);
template <class T> inline void swap(T &a, T &b){T t=a;a=b;b=t;};
template <class T> void sort(T *arr, int begin, int end);
template <class T1, class T2> void sort(T1 *arr, T2 *matchingArray, 
					int begin, int end);

// STATISTICS-RELATED ROUTINES
template <class T> T absval(T value);
template <class T> void findMinMax(const T *array, const int size, 
				   T &min, T &max);
template <class T> float findMean(T *array, int size);
template <class T> float findStddev(T *array, int size);
template <class T> T findMedian(T *array, int size);
template <class T> T findMADFM(T *array, int size);
template <class T> void findMedianStats(T *array, int size, 
					T &median, T &madfm);
template <class T> void findMedianStats(T *array, int size, bool *isGood, 
					T &median, T &madfm);
template <class T> void findNormalStats(T *array, int size, 
					float &mean, float &stddev);
template <class T> void findNormalStats(T *array, int size, bool *isGood, 
		     float &mean, float &stddev);


//--------------------
// PLOTTING ROUTINES
//--------------------
// The following are in plottingUtilities.cc
//
void plotLine(const float slope, const float intercept);
void lineOfEquality();
void lineOfBestFit(int size, float *x, float *y);
void lineOfBestFitPB(const int size, const float *x, const float *y);
void plotVertLine(const float xval, const int colour, const int style);
void plotVertLine(const float xval);
void plotVertLine(const float xval, const int colour);
void plotHorizLine(const float yval, const int colour, const int style);
void plotHorizLine(const float yval);
void plotHorizLine(const float yval, const int colour);
void drawContours(const int size, const float *x, const float *y);

// POSITION-RELATED ROUTINES
std::string getIAUNameEQ(double ra, double dec, float equinox);
std::string getIAUNameGAL(double ra, double dec);
std::string decToDMS(double dec, std::string type);
double dmsToDec(std::string dms);
double angularSeparation(double &ra1, double &dec1, double &ra2, double &dec2);

// WCS-RELATED ROUTINES
int wcsToPixSingle(struct wcsprm *wcs, const double *world, double *pix);
int pixToWCSSingle(struct wcsprm *wcs, const double *pix, double *world);
int wcsToPixMulti(struct wcsprm *wcs, const double *world, 
		  double *pix, const int npts);
int pixToWCSMulti(struct wcsprm *wcs, const double *pix, 
		  double *world, const int npts);
double pixelToVelocity(struct wcsprm *wcs, double &x, double &y, 
		       double &z, std::string velUnits);
double coordToVel(struct wcsprm *wcs, const double coord, std::string outputUnits);
double velToCoord(struct wcsprm *wcs, const float velocity, std::string outputUnits);

// FILTER SMOOTHING ROUTINES
void waveletSmooth(int dim,float *array, int arraySize, float sigma);
void hanningSmooth(float *array, int arraySize, int hanningSize);
void tophatSmooth(float *array, int arraySize, int width);

#endif
