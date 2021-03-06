// -----------------------------------------------------------------------
// utils.hh: Prototypes for utility functions.
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
#ifndef UTILS_H
#define UTILS_H

#include <wcslib/wcs.h>
#include <string>
#include <vector>

struct wcsprm; // just foreshadow this.

// define the speed of light for WCS-related accessor functions 
const float C_kms = 299792.458;

int linear_regression(int num, float *x, float *y, int ilow, int ihigh, 
		      float &slope, float &errSlope, 
		      float &intercept, float &errIntercept, float &r);
template <class Type> void zscale(long imagesize, Type *image, Type &z1, Type &z2);
template <class Type> void zscale(long imagesize, Type *image, Type &z1, Type &z2, Type nullVal);
template <class T> void sort(T *arr, int begin, int end);
template <class T1, class T2> void sort(T1 *arr, T2 *matchingArray, 
					int begin, int end);

// STATISTICS-RELATED ROUTINES
template <class T> T absval(T value);
template <class T> void findMinMax(const T *array, const size_t size, 
				   T &min, T &max);
template <class T> float findMean(T *array, size_t size);
template <class T> float findMean(T *array, std::vector<bool> mask, size_t size);
template <class T> float findMeanDiff(T *first, T *second, size_t size);
template <class T> float findMeanDiff(T *first, T *second, std::vector<bool> mask, size_t size);
template <class T> float findStddev(T *array, size_t size);
template <class T> float findStddev(T *array, std::vector<bool> mask, size_t size);
template <class T> float findStddevDiff(T *first, T *second, size_t size);
template <class T> float findStddevDiff(T *first, T *second, std::vector<bool> mask, size_t size);
template <class T> T findMedian(T *array, size_t size, bool changeArray=false);
template <class T> T findMedian(T *array, std::vector<bool> mask, size_t size);
template <class T> T findMedianDiff(T *first, T *second, size_t size);
template <class T> T findMedianDiff(T *first, T *second, std::vector<bool> mask, size_t size);
template <class T> T findMADFM(T *array, size_t size, bool changeArray=false);
template <class T> T findMADFM(T *array, std::vector<bool> mask, size_t size);
template <class T> T findMADFM(T *array, size_t size, T median, bool changeArray=false);
template <class T> T findMADFM(T *array, std::vector<bool> mask, size_t size, T median);
template <class T> T findMADFMDiff(T *first, T *second, size_t size);
template <class T> T findMADFMDiff(T *first, T *second, std::vector<bool> mask, size_t size);
template <class T> T findMADFMDiff(T *first, T *second, size_t size, T median);
template <class T> T findMADFMDiff(T *first, T *second, std::vector<bool> mask, size_t size, T median);
template <class T> void findMedianStats(T *array, size_t size, 
					T &median, T &madfm);
template <class T> void findMedianStats(T *array, size_t size, std::vector<bool> isGood, 
					T &median, T &madfm);
template <class T> void findNormalStats(T *array, size_t size, 
					float &mean, float &stddev);
template <class T> void findNormalStats(T *array, size_t size, std::vector<bool> isGood, 
					float &mean, float &stddev);
template <class T> void findAllStats(T *array, size_t size, 
				     float &mean, float &stddev,
				     T &median, T &madfm);
template <class T> void findAllStats(T *array, size_t size, std::vector<bool> mask, 
				     float &mean, float &stddev,
				     T &median, T &madfm);
template <class T> void findMedianStatsDiff(T *first, T *second, size_t size, T &median, T &madfm);
template <class T> void findMedianStatsDiff(T *first, T *second, size_t size, std::vector<bool> isGood, T &median, T &madfm);
template <class T> void findNormalStatsDiff(T *first, T *second, size_t size, float &mean, float &stddev);
template <class T> void findNormalStatsDiff(T *first, T *second, size_t size, std::vector<bool> isGood, float &mean, float &stddev);


// POSITION-RELATED ROUTINES
std::string getIAUNameEQ(double ra, double dec, float equinox);
std::string getIAUNameGAL(double ra, double dec);
std::string decToDMS(double dec, std::string type, int decPrecision=2);
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

void findMedianBaseline(int dim, float *inputArray, unsigned int boxWidth, float *baselineValues);

// STRING AND INPUT PARAMETER MANIPULATION ROUTINES
std::string makelower( std::string s );
std::string stringize(bool b);
bool boolify(std::string s);
std::string readSval(std::stringstream& ss);
std::string readFilename(std::stringstream& ss);
bool readFlag(std::stringstream& ss);
float readFval(std::stringstream& ss);
int readIval(std::stringstream& ss);
std::string removeLeadingBlanks(std::string s);
std::string deblank(std::string s);
std::string readStringFromBinaryFile(std::ifstream &infile);
void writeStringToBinaryFile(std::ofstream &outfile, std::string str);
std::vector<int> selectionToIntVec(std::string &str);

// PLOTTING UTILITY FUNCTIONS
void wcsAxes(struct wcsprm *wcs, size_t *axes, int textColour, int axisColour);

// Random number generators -- all in get_random_spectrum.cc
void getRandomSpectrum(int length, float *x, float *y);
void getRandomSpectrum(int length, float *x, double *y);
void getRandomSpectrum(int length, float mean, float sigma, 
		       float *x, double *y);
void getRandomSpectrum(int length, float mean, float sigma, 
		       float *x, float *y);
float getNormalRV();
float getNormalRVtrunc();
float getNormalRV(float mean, float sigma);

#endif
