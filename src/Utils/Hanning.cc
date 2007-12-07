// -----------------------------------------------------------------------
// Hanning.cc: Member functions for the Hanning class.
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
#include <math.h>
#include <duchamp/Utils/Hanning.hh>

Hanning::Hanning(){
  allocated=false;
}

Hanning::~Hanning(){
  if(allocated) delete [] coeffs;
}

Hanning::Hanning(const Hanning& h)
{
  operator=(h);
}

Hanning& Hanning::operator=(const Hanning& h)
{
  if(this==&h) return *this;
  this->hanningSize = h.hanningSize;
  this->allocated = h.allocated;
  if(h.allocated){
    this->coeffs = new float[this->hanningSize];
    for(int i=0;i<h.hanningSize;i++) this->coeffs[i] = h.coeffs[i];
  }
  return *this;
}

Hanning::Hanning(int size){
  /**
   * Constructor that sets the Hanning width and calculates the
   * coefficients. Does this by simply calling the
   * Hanning::define(int) function.
   * \param size The full width of the filter. The parameter \f$a\f$
   * is defined as (size+1)/2.
   */ 

  this->allocated=false;
  this->define(size);
};

void Hanning::define(int size)
{
  /**
   * Function that sets the Hanning width and calculates the coefficients.
   * \param size The full width of the filter. The parameter \f$a\f$ is 
   *  defined as (size+1)/2.
   */ 
  if(size%2==0){ 
    std::cerr << "Hanning: need an odd number for the size. "
	      << "Changing "<< size << " to " << size+1<<".\n";
    size++;
  }
  this->hanningSize = size;
  if(this->allocated) delete [] this->coeffs;
  this->coeffs = new float[size];
  this->allocated = true;
  float a = (size+1.)/2.;
  for(int i=0;i<size;i++){
    float x = i-(size-1)/2.;
    this->coeffs[i] = 0.5 + 0.5*cos(x * M_PI / a);
  }
}



float *Hanning::smooth(float *array, int npts){
  /**
   * This smooths an array of float by doing a discrete convolution of
   * the input array with the filter coefficients.
   * 
   * Currently only works for C arrays of floats, as that is all I
   * need it for.  Could be made more general if needs be.
   *
   * \param array The input array. Needs to be defined -- no memory
   * checks are done!  
   * \param npts The size of the input array.
   * \return Returns an array of the same size. If filter coefficients
   * have not been allocated, the input array is returned.
   */
  if(!this->allocated) return array;
  else{
    float *newarray = new float[npts];
    float scale = (hanningSize+1.)/2.;
    for(int i=0;i<npts;i++){
      newarray[i] = 0.;
      for(int j=0;j<hanningSize;j++){
	int x = j-(hanningSize-1)/2;
	if((i+x>0)&&(i+x<npts)) newarray[i]+=coeffs[j] * array[i+x];
      }
      newarray[i] /= scale;
    }
    return newarray;
  }
}
