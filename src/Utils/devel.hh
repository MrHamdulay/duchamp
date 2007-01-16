#ifndef TESTS_HH
#define TESTS_HH

// MENU ROUTINES FOR DIGANOSTIC/TEST PROGRAMS
std::string menu();
std::string specMenu();
std::string orionMenu();
std::string imageMenu();
std::string twoblMenu();

// trimmed histogram statistics -- in trimStats.cc
void findTrimmedHistStats(float *array, const int size, float &tmean, float &tsigma);
void findTrimmedHistStatsOLD(float *array, const int size, float &tmean, float &tsigma)
void findTrimmedHistStats2(float *array, const int size, float &tmean, float &tsigma)

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
