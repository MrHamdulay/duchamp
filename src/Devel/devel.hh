// -----------------------------------------------------------------------
// devel.hh: Prototypes for development functions.
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
#ifndef TESTS_HH
#define TESTS_HH
#include <vector>
#include <string>
#include <duchamp/param.hh>
#include <duchamp/ATrous/filter.hh>

// MENU ROUTINES FOR DIGANOSTIC/TEST PROGRAMS
std::string menu();
std::string specMenu();
std::string orionMenu();
std::string imageMenu();
std::string twoblMenu();
std::string b1555Menu();
void spectralSelection(std::vector<float> &xvalues, 
		       std::vector<float> &yvalues, 
		       size_t &zdim);

// trimmed histogram statistics -- in trimStats.cc
void findTrimmedHistStats(float *array, const int size, float &tmean, float &tsigma);
void findTrimmedHistStatsOLD(float *array, const int size, float &tmean, float &tsigma);
void findTrimmedHistStats2(float *array, const int size, float &tmean, float &tsigma);

// Atrous tranform functions not used in duchamp code
namespace duchamp {
  class Param;
  void atrousTransform(size_t &length, int &numScales, float *spectrum, double *coeffs, double *wavelet, Param &par);
  void atrousTransform(size_t &length, float *spectrum, float *coeffs, float *wavelet, Param &par);
  void atrousTransform2D(size_t &xdim, size_t &ydim, int &numScales, float *input, double *coeffs, double *wavelet, Param &par);
  void atrousTransform2D(size_t &xdim, size_t &ydim, int &numScales, float *input, double *coeffs, double *wavelet);
  void atrousTransform3D(size_t &xdim, size_t &ydim, size_t &zdim, int &numScales, float *&input, float *&coeffs, float *&wavelet, Param &par);
  void atrousTransform3D(size_t &xdim, size_t &ydim, size_t &zdim, int &numScales, float *input, float *coeffs, float *wavelet);
}

// Calculating the sigma factors for the atrous reconstruction
//    -- in sigma_factors.cc
void getSigmaFactors(int &numScales, float *factors);
void getSigmaFactors2D(int &numScales, float *factors);
void getSigmaFactors3D(int &numScales, float *factors);
void getSigmaFactors1DNew(duchamp::Filter &reconFilter, int &numScales);
void getSigmaFactors2DNew(duchamp::Filter &reconFilter, int &numScales);
void getSigmaFactors3DNew(duchamp::Filter &reconFilter, int &numScales);


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

// The following are in plotImage.cc
//
void plotImage(float *array, int xdim, int ydim);
void plotImage(float *array, int xdim, int ydim, duchamp::Param par);
void plotImage(float *array, int xdim, int ydim, float z1, float z2);



#endif // TESTS_HH
