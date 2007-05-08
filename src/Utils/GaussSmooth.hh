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
  float *smooth(float *input, int xdim, int ydim);  
  /** Smooth an array with the Gaussian kernel, using a mask to define
      blank pixels*/
  float *smooth(float *input, int xdim, int ydim, bool *mask);  
  
  void   setKernMaj(float f){kernMaj=f;};
  void   setKernMin(float f){kernMin=f;};
  void   setKernPA(float f){kernPA=f;};

  int    getKernWidth(){return kernWidth;};
  float  getStddevScale(){return stddevScale;};

private:
  float  kernMaj;      ///< The FWHM of the major axis of the
		       ///   elliptical Gaussian.
  float  kernMin;      ///< The FWHM of the minor axis of the
		       ///   elliptical Gaussian.
  float  kernPA;       ///< The position angle of the elliptical
		       ///   Gaussian.
  int    kernWidth;    ///< The width of the kernel (in pixels).
  float  stddevScale;  ///< The factor by which the rms of the input
		       ///   array gets scaled by (assuming iid
		       ///   normally)
  float *kernel;       ///< The coefficients of the smoothing kernel
  bool   allocated;    ///< Have the coefficients been allocated in memory?

};

#endif  // HANNING_H
