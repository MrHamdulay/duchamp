#ifndef GAUSSSMOOTH_H
#define GAUSSSMOOTH_H

/**
 *  Define a Gaussian to smooth a 2D array.
 *
 *  A simple class to define a Gaussian kernel that can be used to
 *  smooth a two-dimensional array.
 */

class GaussSmooth
{
public:
  GaussSmooth();          ///< Basic constructor: no kernel defined.
  virtual ~GaussSmooth(); ///< Destructor

  /** Specific constructor that sets up kernel.*/
  GaussSmooth(float maj, float min, float pa);  
  /** Specific constructor that sets up kernel: assuming circular gaussian.*/
  GaussSmooth(float maj);  

  /** Define the size and the array of coefficients. */
  void   define(float maj, float min, float pa); 

  /** Smooth an array with the Gaussian kernel*/
  float *smooth(float *array, int xdim, int ydim);  
  /** Smooth an array with the Gaussian kernel, using a mask to define
      blank pixels*/
  float *smooth(float *array, int xdim, int ydim, bool *mask);  
  
  void   setKernMaj(float f){kernMaj=f;};
  void   setKernMin(float f){kernMin=f;};
  void   setKernPA(float f){kernPA=f;};

  int    getKernWidth(){return kernWidth;};
  float  getStddevScale(){return stddevScale;};

private:
  float  kernMaj;
  float  kernMin;
  float  kernPA;
  int    kernWidth;
  float  stddevScale;
  float *kernel;       ///< The coefficients of the smoothing kernel
  bool   allocated;    ///< Have the coefficients been allocated in memory?

};

#endif  // HANNING_H
