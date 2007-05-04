#ifndef TESTS_HH
#define TESTS_HH
#include <vector>
#include <string>

class Param;

// MENU ROUTINES FOR DIGANOSTIC/TEST PROGRAMS
std::string menu();
std::string specMenu();
std::string orionMenu();
std::string imageMenu();
std::string twoblMenu();
void spectralSelection(std::vector<float> &xvalues, 
		       std::vector<float> &yvalues, 
		       long &zdim);

// trimmed histogram statistics -- in trimStats.cc
void findTrimmedHistStats(float *array, const int size, float &tmean, float &tsigma);
void findTrimmedHistStatsOLD(float *array, const int size, float &tmean, float &tsigma);
void findTrimmedHistStats2(float *array, const int size, float &tmean, float &tsigma);

// Atrous tranform functions not used in duchamp code
void atrousTransform(long &length, int &numScales, float *spectrum, double *coeffs, double *wavelet, Param &par);
void atrousTransform(long &length, float *spectrum, float *coeffs, float *wavelet, Param &par);
void atrousTransform2D(long &xdim, long &ydim, int &numScales, float *input, double *coeffs, double *wavelet, Param &par);
void atrousTransform2D(long &xdim, long &ydim, int &numScales, float *input, double *coeffs, double *wavelet);
void atrousTransform3D(long &xdim, long &ydim, long &zdim, int &numScales, float *&input, float *&coeffs, float *&wavelet, Param &par);
void atrousTransform3D(long &xdim, long &ydim, long &zdim, int &numScales, float *input, float *coeffs, float *wavelet);


// Calculating the sigma factors for the atrous reconstruction
//    -- in sigma_factors.cc
void getSigmaFactors(int &numScales, float *factors);
void getSigmaFactors2D(int &numScales, float *factors);
void getSigmaFactors3D(int &numScales, float *factors);
void getSigmaFactors1DNew(int &numScales);
void getSigmaFactors2DNew(int &numScales);
void getSigmaFactors3DNew(int &numScales);


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




#endif // TESTS_HH
