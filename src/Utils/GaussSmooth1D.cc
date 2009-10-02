// -----------------------------------------------------------------------
// GaussSmooth1D.cc: Member functions for the GaussSmooth1D class.
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
#include <iostream>
#include <duchamp/config.h>
#include <math.h>
#ifdef HAVE_VALUES_H
#include <values.h>
#endif
#ifdef MAXFLOAT
#define MAXVAL MAXFLOAT
#else
#define MAXVAL 1.e38F
#endif
#include <duchamp/Utils/GaussSmooth1D.hh>

template <class Type>
GaussSmooth1D<Type>::GaussSmooth1D()
{
  allocated=false;
}
template GaussSmooth1D<float>::GaussSmooth1D();
template GaussSmooth1D<double>::GaussSmooth1D();

template <class Type>
GaussSmooth1D<Type>::~GaussSmooth1D()
{
  if(allocated) delete [] kernel;
}
template GaussSmooth1D<float>::~GaussSmooth1D();
template GaussSmooth1D<double>::~GaussSmooth1D();

template <class Type>
GaussSmooth1D<Type>::GaussSmooth1D(const GaussSmooth1D& g)
{
  operator=(g);
}
template GaussSmooth1D<float>::GaussSmooth1D(const GaussSmooth1D& g);
template GaussSmooth1D<double>::GaussSmooth1D(const GaussSmooth1D& g);

template <class Type>
GaussSmooth1D<Type>& GaussSmooth1D<Type>::operator=(const GaussSmooth1D& g)
{
  if(this==&g) return *this;
  this->kernFWHM = g.kernFWHM;
  this->kernWidth = g.kernWidth;
  this->stddevScale = g.stddevScale;
  if(this->allocated) delete [] this->kernel;
  this->allocated = g.allocated;
  if(this->allocated){
    this->kernel = new Type[this->kernWidth*this->kernWidth];
    for(int i=0;i<this->kernWidth*this->kernWidth;i++)
      this->kernel[i] = g.kernel[i];
  }
  return *this;
}
template GaussSmooth1D<float>& GaussSmooth1D<float>::operator=(const GaussSmooth1D& g);
template GaussSmooth1D<double>& GaussSmooth1D<double>::operator=(const GaussSmooth1D& g);

template <class Type>
GaussSmooth1D<Type>::GaussSmooth1D(float fwhm)
{
  this->allocated=false;
  this->define(fwhm);
}
template GaussSmooth1D<float>::GaussSmooth1D(float fwhm);
template GaussSmooth1D<double>::GaussSmooth1D(float fwhm);

template <class Type>
void GaussSmooth1D<Type>::define(float fwhm)
{

  this->kernFWHM = fwhm;

  // The parameter kernFWHM is the full-width-at-half-maximum of the
  // Gaussian. We correct this to the sigma parameter for the 1D
  // gaussian by halving and dividing by sqrt(2 ln(2)). Actually work
  // with sigma_x^2 to make things easier.
  float sigma2 = (this->kernFWHM*this->kernFWHM/4.) / (2.*M_LN2);

  // First determine the size of the kernel.  Calculate the size based
  // on the number of pixels needed to make the exponential drop to
  // less than the minimum floating-point value. Use the major axis to
  // get the largest square that includes the ellipse.
  float sigma = this->kernFWHM / (4.*M_LN2);
  int kernelHW = int(ceil(sigma * sqrt(-2.*log(1. / MAXVAL))));
  this->kernWidth = 2*kernelHW + 1;

  if(this->allocated) delete [] this->kernel;
  this->kernel = new Type[this->kernWidth];
  this->allocated = true;
  this->stddevScale=0.;

  float sum=0.;
  for(int i=0;i<this->kernWidth;i++){
    float xpt = (i-kernelHW);
    float rsq = (xpt*xpt/sigma2);
    kernel[i] = exp( -0.5 * rsq);
    sum += kernel[i];
    this->stddevScale += kernel[i]*kernel[i];
  }
  for(int i=0;i<this->kernWidth;i++) kernel[i] /= sum;
  this->stddevScale = sqrt(this->stddevScale);
}
template void GaussSmooth1D<float>::define(float fwhm);
template void GaussSmooth1D<double>::define(float fwhm);

template <class Type>
Type *GaussSmooth1D<Type>::smooth(Type *input, int dim, bool normalise, bool scaleByCoverage)
{
  /// @details Smooth a given one-dimensional array, of dimension dim,
  /// with a gaussian. Simply runs as a front end to
  /// GaussSmooth1D::smooth(float *, int, bool *) by defining a mask
  /// that allows all pixels in the input array.
  /// 
  ///  \param input The 2D array to be smoothed.
  ///  \param dim  The size of the x-dimension of the array.
  ///  \return The smoothed array.

  Type *smoothed;
  bool *mask = new bool[dim];
  for(int i=0;i<dim;i++) mask[i]=true;
  smoothed = this->smooth(input,dim,mask,scaleByCoverage);
  delete [] mask;
  return smoothed;
}
template float *GaussSmooth1D<float>::smooth(float *input, int dim, bool normalise, bool scaleByCoverage);
template double *GaussSmooth1D<double>::smooth(double *input, int dim, bool normalise, bool scaleByCoverage);

template <class Type>
Type *GaussSmooth1D<Type>::smooth(Type *input, int dim, bool *mask, bool normalise, bool scaleByCoverage)
{
  /// @details Smooth a given one-dimensional array, of dimension dim,
  ///  with a gaussian, where the boolean array mask defines which
  ///  values of the array are valid.
  /// 
  ///  This function convolves the input array with the kernel that
  ///  needs to have been defined. If it has not, the input array is
  ///  returned unchanged.
  /// 
  ///  The mask should be the same size as the input array, and have
  ///  values of true for entries that are considered valid, and false
  ///  for entries that are not. For instance, arrays from FITS files
  ///  should have the mask entries corresponding to BLANK pixels set
  ///  to false.
  /// 
  ///  \param input The 2D array to be smoothed.
  ///  \param dim  The size of the x-dimension of the array.
  ///  \param mask The array showing which pixels in the input array
  ///              are valid.
  ///  \return The smoothed array.

  if(!this->allocated) return input;
  else{

    Type *output = new Type[dim];

    int comp,fpos,ct;
    float fsum,kernsum=0;
    int kernelHW = this->kernWidth/2;

    for(int i=0;i<this->kernWidth;i++)
      kernsum += this->kernel[i];
    
    for(int pos = 0; pos<dim; pos++){
      
      if(!mask[pos]) output[pos] = input[pos];
      else{
	
	ct=0;
	fsum=0.;
	output[pos] = 0.;
	
	for(int off = -kernelHW; off<=kernelHW; off++){
	  comp = pos + off;	      
	  if((comp>=0)&&(comp<dim)){
	    fpos = (off+kernelHW);
	    if(mask[comp]){
	      ct++;
	      fsum += this->kernel[fpos];
	      output[pos] += input[comp]*this->kernel[fpos];
	    }

	  }
	} // off loop
	
	if(ct>0){
	  if(scaleByCoverage) output[pos] /= fsum;
	  if(normalise) output[pos] /= kernsum;
	}

      } // else{

    } //pos loop
   
    return output;
  }
  
}
template float *GaussSmooth1D<float>::smooth(float *input, int dim, bool *mask, bool normalise, bool scaleByCoverage);
template double *GaussSmooth1D<double>::smooth(double *input, int dim, bool *mask, bool normalise, bool scaleByCoverage);
