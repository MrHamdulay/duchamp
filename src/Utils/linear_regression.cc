#include <iostream>
#include <math.h>
int linear_regression(int num, float *x, float *y, int ilow, int ihigh, float &slope, float &errSlope, float &intercept, float &errIntercept, float &r)
{
  /**
   *  int linear_regression()
   *
   * Computes the linear best fit to data - y = a*x + b, where x and y are 
   * arrays of size num, a is the slope and b the y-intercept.
   * The values used in the arrays are those from ilow to ihigh. 
   * (ie. if the full arrays are being used, then ilow=0 and ihigh=num-1.
   * Returns the values of slope & intercept (with errors) 
   * as well as r, the regression coefficient. 
   * If everything works, returns 0.
   * If slope is infinite (eg, all points have same x value), returns 1.
   */

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
