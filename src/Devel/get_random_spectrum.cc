// -----------------------------------------------------------------------
// get_random_spectrum.cc: Functions to obtain random values.
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

float getNormalRV()
{
  float v1,v2,s;
  // simulate a standard normal RV via polar method
  do{
    v1 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
    v2 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
    s = v1*v1+v2*v2;
  }while(s>1);
  return sqrt(-2.*log(s)/s)*v1;

}

float getNormalRVtrunc()
{
  float v1,v2,s;
  // simulate a standard normal RV via polar method
  do{
    v1 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
    v2 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
    s = v1*v1+v2*v2;
  }while((s>1)||(fabs(sqrt(-2.*log(s)/s)*v1)>sqrt(2*M_LN2)));
  return sqrt(-2.*log(s)/s)*v1;

}

float getNormalRV(float mean, float sigma)
{
  // simulate a standard normal RV via polar method
  float v1,v2,s;
  do{
    v1 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
    v2 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
    s = v1*v1+v2*v2;
  }while(s>1);
  float z=sqrt(-2.*log(s)/s)*v1;
  return z*sigma + mean;
}

void getRandomSpectrum(int length, float *x, float *y)
{
  srandom(time(0));
  rand();
  for(int i=0;i<length;i++){
    x[i] = (float)i;
    // simulate a standard normal RV via polar method
    double v1,v2,s;
    do{
      v1 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
      v2 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
      s = v1*v1+v2*v2;
    }while(s>1);
    y[i] = sqrt(-2.*log(s)/s)*v1;
  }
}

void getRandomSpectrum(int length, float *x, double *y)
{
  srandomdev();
  rand();
  for(int i=0;i<length;i++){
    x[i] = (float)i;
    // simulate a standard normal RV via polar method
    double v1,v2,s;
    do{
      v1 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
      v2 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
      s = v1*v1+v2*v2;
    }while(s>1);
    y[i] = sqrt(-2.*log(s)/s)*v1;
  }
}


void getRandomSpectrum(int length, float mean, float sigma, 
		       float *x, double *y)
{
  srandomdev();
  rand();
  for(int i=0;i<length;i++){
    x[i] = (float)i;
    // simulate a standard normal RV via polar method
    double v1,v2,s;
    do{
      v1 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
      v2 = 2.*(1.*random())/(RAND_MAX+1.0) - 1.;
      s = v1*v1+v2*v2;
    }while(s>1);
    float z = sqrt(-2.*log(s)/s)*v1;
    y[i] = z * sigma + mean;
  }
}

