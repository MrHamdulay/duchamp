#ifndef UTILS_H
#define UTILS_H

#include <wcs.h>
#include <string>
using std::string;
// #include <functional>
// #include <algorithm>
// #include <iterator>

// define the speed of light for WCS-related accessor functions 
const float C_kms = 299792.458;

// Divide by the following correction factor to convert from MADFM to sigma estimator.
const float correctionFactor = 0.6744888;

// MENU ROUTINES FOR DIGANOSTIC/TEST PROGRAMS
string menu();
string specMenu();
string orionMenu();
string imageMenu();
string twoblMenu();

void linear_regression(int num, float *x, float *y, int ilow, int ihigh, float &slope, float &errSlope, float &intercept, float &errIntercept, float &r);
void zscale(long imagesize, float *image, float &z1, float &z2);
void zscale(long imagesize, float *image, float &z1, float &z2, float nullVal);
void swap(float &a, float &b);
void sort(float *arr, int begin, int end);
void sort(float *arr, float *matchingArray, int begin, int end);
// template< typename BidirectionalIterator, typename Compare >
// void quick_sort( BidirectionalIterator first, BidirectionalIterator last, Compare cmp );
// template< typename BidirectionalIterator >
// inline void quick_sort( BidirectionalIterator first, BidirectionalIterator last );

// STATISTICS-RELATED ROUTINES
float findMean(float *&array, int size);
float findSigma(float *&array, int size);
float findMedian(float *&array, int size);
float findMADFM(float *&array, int size);
void findMedianStats(float *&array, int size, float &median, float &madfm);
void findMedianStats(float *&array, long size, float &median, float &madfm);
void findNormalStats(float *&array, int size, float &mean, float &sig);
void getRandomSpectrum(int length, float *x, float *y);
void getRandomSpectrum(int length, float *x, double *y);
void getRandomSpectrum(int length, float mean, float sigma, float *x, double *y);
void getRandomSpectrum(int length, float mean, float sigma, float *x, float *y);
float getNormalRV();
float getNormalRV(float mean, float sigma);
void getSigmaFactors(int &numScales, float *factors);
void getSigmaFactors2D(int &numScales, float *factors);
void getSigmaFactors3D(int &numScales, float *factors);
void getSigmaFactors1DNew(int &numScales);
void getSigmaFactors2DNew(int &numScales);
void getSigmaFactors3DNew(int &numScales);


// PLOTTING ROUTINES
void cpgwedglog(const char* side, float disp, float width, float fg, float bg, const char *label);
void cpghistlog(int npts, float *data, float datamin, float datamax, int nbin, int pgflag);
void cpgcumul(int npts, float *data, float datamin, float datamax, int pgflag);
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
string getIAUNameEQ(double ra, double dec, float equinox);
string getIAUNameGAL(double ra, double dec);
string decToDMS(double dec, string type);
double dmsToDec(string dms);
double angularSeparation(double &ra1, double &dec1, double &ra2, double &dec2);

// WCS-RELATED ROUTINES
double pixelToVelocity(wcsprm *wcs, double &x, double &y, double &z);
int wcsToPixSingle(wcsprm *wcs, const double *world, double *pix);
int pixToWCSSingle(wcsprm *wcs, const double *pix, double *world);
int wcsToPixMulti(wcsprm *wcs, const double *world, double *pix, const int npts);
int pixToWCSMulti(wcsprm *wcs, const double *pix, double *world, const int npts);
float setVel_kms(wcsprm *wcs, const double coord);
double velToCoord(wcsprm *wcs, const float velocity);

// FILTER SMOOTHING ROUTINES
void waveletSmooth(int dim,float *array, int arraySize, float sigma);
void hanningSmooth(float *array, int arraySize, int hanningSize);
void tophatSmooth(float *array, int arraySize, int width);

#endif
