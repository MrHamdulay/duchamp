// -----------------------------------------------------------------------
// interpolation.cc: Intepolate in or extrapolate from an array to
//                     find values
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
#include <duchamp/Utils/utils.hh>

float interpolateToFindX(float yval, float *x, float *y, int size, int min, int max)
{
  if(min<0) min=0;
  if(max<0) max=size-1;

  int i=min+1;
  while( ((y[i]-yval)*(y[min]-yval) > 0) && i<max) i++;
  if(i==max){
    if(y[min]>y[max]){ if(y[min]>yval) i=max; else i=min; }
    else{ if(y[min]>yval) i=min; else i=max; }
  }
  float slope = (y[i]-y[i-1])/(x[i]-x[i-1]);
  float xval = x[i-1] + (yval-y[i-1])/slope;
  return xval;
}


float interpolateToFindY(float xval, float *x, float *y, int size, int min, int max)
{
  if(min<0) min=0;
  if(max<0) max=size;

  int i=min+1;
  while( ((x[i]-xval)*(x[i-1]-xval)>0) && i<max) i++;
  float slope = (y[i]-y[i-1])/(x[i]-x[i-1]);
  float yval = y[i-1] + (xval-x[i-1])*slope;
  return yval;
}
