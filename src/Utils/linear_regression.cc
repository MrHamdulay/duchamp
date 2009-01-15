// -----------------------------------------------------------------------
// linear_regression.cc: Performs linear regression on a set of (x,y)
//                       values.
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
int linear_regression(int num, float *x, float *y, int ilow, int ihigh, float &slope, float &errSlope, float &intercept, float &errIntercept, float &r)
{
  /// @details
  /// Computes the linear best fit to data: $y=a x + b$, where $x$ and
  /// $y$ are arrays of size num, $a$ is the slope and $b$ the
  /// y-intercept.  The values used in the arrays are those from ilow
  /// to ihigh.  (ie. if the full arrays are being used, then ilow=0
  /// and ihigh=num-1.  Returns the values of slope & intercept (with
  /// errors) as well as r, the regression coefficient.
  /// 
  /// \param num Size of the x & y arrays.
  /// \param x   Array of abscissae.
  /// \param y   Array of ordinates.
  /// \param ilow Minimum index of the arrays to be used (ilow=0 means
  ///             start at the beginning).
  /// \param ihigh Maximum index of the arrays to be used (ihigh=num-1
  ///              means finish at the end).
  /// \param slope Returns value of the slope of the best fit line.
  /// \param errSlope Returns value of the estimated error in the slope
  ///                  value.
  /// \param intercept Returns value of the y-intercept of the best fit
  ///                  line.
  /// \param errIntercept Returns value of the estimated error in the
  ///                     value of the y-intercept.
  /// \param r Returns the value of the regression coefficient.
  /// \return If everything works, returns 0. If slope is infinite (eg,
  /// all points have same x value), returns 1.
  /// 

  if (ilow>ihigh) {
    std::cerr << "Error! linear_regression.cc :: ilow (" << ilow 
	      << ") > ihigh (" << ihigh << ")!!\n";
    return 1;
  }
  if (ihigh>num-1) {
    std::cerr << "Error! linear_regression.cc :: ihigh (" <<ihigh
	      << ") out of bounds of array (>" << num-1 << ")!!\n";
    return 1;
  }
  if(ilow<0){
    std::cerr << "Error! linear_regression.cc :: ilow (" << ilow
	      << ") < 0. !!\n";
    return 1;
  }

  double sumx,sumy,sumxx,sumxy,sumyy;
  sumx=0.;
  sumy=0.;
  sumxx=0.;
  sumxy=0.;
  sumyy=0.;
  int count=0;
  for (int i=ilow;i<=ihigh;i++){
    count++;
    sumx = sumx + x[i];
    sumy = sumy + y[i];
    sumxx = sumxx + x[i]*x[i];
    sumxy = sumxy + x[i]*y[i];
    sumyy = sumyy + y[i]*y[i];
  }

  const float SMALLTHING=1.e-6;
  if(fabs(count*sumxx-sumx*sumx)<SMALLTHING) return 1;
  else{

    slope = (count*sumxy - sumx*sumy)/(count*sumxx - sumx*sumx);
    errSlope = count / (count*sumxx - sumx*sumx);

    intercept = (sumy*sumxx - sumxy*sumx)/(count*sumxx - sumx*sumx);
    errIntercept = sumxx / (count*sumxx - sumx*sumx);
    
    r = (count*sumxy - sumx*sumy) /
      (sqrt(count*sumxx-sumx*sumx) * sqrt(count*sumyy-sumy*sumy) );

    return 0;

  }
}
