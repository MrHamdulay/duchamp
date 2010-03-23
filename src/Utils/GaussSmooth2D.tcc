// -----------------------------------------------------------------------
// GaussSmooth2D.tcc: Member functions for the GaussSmooth2D class.
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
#include <sstream>
#include <duchamp/duchamp.hh>
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
#include <duchamp/Utils/GaussSmooth2D.hh>

template <class Type>
void GaussSmooth2D<Type>::defaults()
{
  this->allocated=false;
  this->blankVal = Type(-99);
  this->kernWidth = -1;
}

template <class Type>
GaussSmooth2D<Type>::GaussSmooth2D()
{
  this->defaults();
}

template <class Type>
GaussSmooth2D<Type>::~GaussSmooth2D()
{
  if(this->allocated) delete [] kernel;
}

template <class Type>
GaussSmooth2D<Type>::GaussSmooth2D(const GaussSmooth2D& g)
{
  operator=(g);
}

template <class Type>
GaussSmooth2D<Type>& GaussSmooth2D<Type>::operator=(const GaussSmooth2D& g)
{
  if(this==&g) return *this;
  this->kernMaj   = g.kernMaj;
  this->kernMin   = g.kernMin;
  this->kernPA    = g.kernPA;
  this->kernWidth = g.kernWidth;
  this->stddevScale = g.stddevScale;
  this->blankVal    = g.blankVal;
  if(this->allocated) delete [] this->kernel;
  this->allocated = g.allocated;
  if(this->allocated){
    this->kernel = new Type[this->kernWidth*this->kernWidth];
    for(int i=0;i<this->kernWidth*this->kernWidth;i++)
      this->kernel[i] = g.kernel[i];
  }
  return *this;
}

template <class Type>
GaussSmooth2D<Type>::GaussSmooth2D(float maj, float min, float pa)
{
  this->defaults();
  this->define(maj, min, pa);
}

template <class Type>
GaussSmooth2D<Type>::GaussSmooth2D(float maj)
{
  this->defaults();
  this->define(maj, maj, 0);
}

template <class Type>
void GaussSmooth2D<Type>::define(float maj, float min, float pa)
{

  this->kernMaj = maj;
  this->kernMin = min;
  this->kernPA  = pa;

  // The parameters kernMaj & kernMin are the FWHM in the major and
  // minor axis directions. We correct these to the sigma_x and
  // sigma_y parameters for the 2D gaussian by halving and dividing by
  // sqrt(2 ln(2)). Actually work with sigma_x^2 to make things
  // easier.
  float sigmaX2 = (this->kernMaj*this->kernMaj/4.) / (2.*M_LN2);
  float sigmaY2 = (this->kernMin*this->kernMin/4.) / (2.*M_LN2);

  // First determine the size of the kernel.  Calculate the size based
  // on the number of pixels needed to make the exponential drop to
  // less than the minimum floating-point value. Use the major axis to
  // get the largest square that includes the ellipse.
  float majorSigma = this->kernMaj / (4.*M_LN2);
  int kernelHW = int(ceil(majorSigma * sqrt(-2.*log(1. / MAXVAL))));

  if(this->kernWidth < 0) this->kernWidth = 2*kernelHW + 1;
  else if(this->kernWidth < 2*kernelHW + 1){
    std::stringstream ss;
    ss << "You have provided a kernel smaller than optimal (" << this->kernWidth << " cf. " << 2*kernelHW + 1 <<")";
    duchamp::duchampWarning("GaussSmooth2D::define",ss.str());
  }

  if(this->allocated) delete [] this->kernel;
  this->kernel = new Type[this->kernWidth*this->kernWidth];
  this->allocated = true;
  this->stddevScale=0.;
  float posang = this->kernPA * M_PI/180.;


  for(int i=0;i<this->kernWidth;i++){
    for(int j=0;j<this->kernWidth;j++){
      float xpt = (i-kernelHW)*sin(posang) - (j-kernelHW)*cos(posang);

      float ypt = (i-kernelHW)*cos(posang) + (j-kernelHW)*sin(posang);
      float rsq = (xpt*xpt/sigmaX2) + (ypt*ypt/sigmaY2);
      kernel[i*this->kernWidth+j] = exp( -0.5 * rsq);
      this->stddevScale += 
	kernel[i*this->kernWidth+j]*kernel[i*this->kernWidth+j];
    }
  }
  this->stddevScale = sqrt(this->stddevScale);
}

template <class Type>
Type *GaussSmooth2D<Type>::smooth(Type *input, int xdim, int ydim, EDGES edgeTreatment)
{
  /// @details
  /// Smooth a given two-dimensional array, of dimensions xdim
  /// \f$\times\f$ ydim, with an elliptical gaussian. Simply runs as a
  /// front end to GaussSmooth2D::smooth(float *, int, int, bool *) by
  /// defining a mask that allows all pixels in the input array.
  /// 
  ///  \param input The 2D array to be smoothed.
  ///  \param xdim  The size of the x-dimension of the array.
  ///  \param ydim  The size of the y-dimension of the array.
  ///  \return The smoothed array.

  Type *smoothed;
  bool *mask = new bool[xdim*ydim];
  for(int i=0;i<xdim*ydim;i++) mask[i]=true;
  smoothed = this->smooth(input,xdim,ydim,mask,edgeTreatment);
  delete [] mask;
  return smoothed;
}

template <class Type>
Type *GaussSmooth2D<Type>::smooth(Type *input, int xdim, int ydim, bool *mask, EDGES edgeTreatment)
{
  /// @details
  ///  Smooth a given two-dimensional array, of dimensions xdim
  ///  \f$\times\f$ ydim, with an elliptical gaussian, where the
  ///  boolean array mask defines which values of the array are valid.
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
  ///  \param xdim  The size of the x-dimension of the array.
  ///  \param ydim  The size of the y-dimension of the array.
  ///  \param mask The array showing which pixels in the input array
  ///              are valid.
  ///  \return The smoothed array.

  if(!this->allocated) return input;
  else{

    Type *output = new Type[xdim*ydim];

    int pos,comp,xcomp,ycomp,fpos,ct;
    float fsum,kernsum=0;
    int kernelHW = this->kernWidth/2;

    for(int i=0;i<this->kernWidth;i++)
      for(int j=0;j<this->kernWidth;j++)
	kernsum += this->kernel[i*this->kernWidth+j];

    for(int ypos = 0; ypos<ydim; ypos++){
      for(int xpos = 0; xpos<xdim; xpos++){
	pos = ypos*xdim + xpos;
      
	if(!mask[pos]) output[pos] = input[pos];
	else if(edgeTreatment==TRUNCATE &&
		((xpos<=kernelHW)||((xdim-xpos)<=kernelHW)||(ypos<=kernelHW)||((ydim-ypos)<kernelHW))){
	  output[pos] = this->blankVal;
	}
	else{
	
	  ct=0;
	  fsum=0.;
	  output[pos] = 0.;
	
	  for(int yoff = -kernelHW; yoff<=kernelHW; yoff++){
	    ycomp = ypos + yoff;
	    if((ycomp>=0)&&(ycomp<ydim)){

	      for(int xoff = -kernelHW; xoff<=kernelHW; xoff++){
		xcomp = xpos + xoff;	      
		if((xcomp>=0)&&(xcomp<xdim)){

		  fpos = (xoff+kernelHW) + (yoff+kernelHW)*this->kernWidth;
		  comp = ycomp*xdim + xcomp;
		  if(mask[comp]){
		    ct++;
		    fsum += this->kernel[fpos];
		    output[pos] += input[comp]*this->kernel[fpos];
		  }

		}
	      } // xoff loop

	    }
	  }// yoff loop
	  // 	  if(ct>0 && scaleByCoverage) output[pos] /= fsum;
 	  if(ct>0 && edgeTreatment==SCALEBYCOVERAGE) output[pos] *= kernsum/fsum;
 
	} // else{

      } //xpos loop
    }   //ypos loop
   
    return output;
  }

}
