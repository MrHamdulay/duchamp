// -----------------------------------------------------------------------
// GaussSmooth2D.hh: Definition of GaussSmooth2D class, used to smooth a
//                 2D image with a Gaussian kernel.
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
#ifndef GAUSSSMOOTH2D_H
#define GAUSSSMOOTH2D_H
#include <vector>
#include <duchamp/duchamp.hh>

/// @brief How the edges of the array are dealt with
enum EDGES { EQUALTOEDGE,       ///< All pixels are used and treated equally
	     SCALEBYCOVERAGE,   ///< All pixels are used, but edge pixels are weighted down by the kernel coverage
	     TRUNCATE           ///< Pixels at edge are set to Blank.
};

/// @brief
///  Define a Gaussian to smooth a 2D array.
/// @details
///  A simple class to define a Gaussian kernel that can be used to
///  smooth a two-dimensional array.

template <class Type>
class GaussSmooth2D
{
public:
  GaussSmooth2D();          ///< Basic constructor: no kernel defined.
  virtual ~GaussSmooth2D(); ///< Destructor
  GaussSmooth2D(const GaussSmooth2D& g);
  GaussSmooth2D& operator=(const GaussSmooth2D& g);
  
  void defaults(); 

  /// @brief Specific constructor that sets up kernel.
  GaussSmooth2D(float maj, float min, float pa, float cutoff);  
  /// @brief Specific constructor that sets up kernel.
  GaussSmooth2D(float maj, float min, float pa);  
  /// @brief Specific constructor that sets up kernel: assuming circular gaussian.
  GaussSmooth2D(float maj);  

  /// @brief Define the size and the array of coefficients. 
  void   define(float maj, float min, float pa, float cutoff); 

  /// @brief Set the size of the kernel
  void   setKernelWidth(float width){kernWidth = width;};

  /// @brief Smooth an array with the Gaussian kernel
  Type *smooth(Type *input, size_t xdim, size_t ydim, EDGES edgeTreatment=EQUALTOEDGE);  
  /// @brief Smooth an array with the Gaussian kernel, using a mask to define blank pixels
  Type *smooth(Type *input, size_t xdim, size_t ydim, std::vector<bool> mask, EDGES edgeTreatment=EQUALTOEDGE);  
  
  void   setKernMaj(float f){kernMaj=f;};
  void   setKernMin(float f){kernMin=f;};
  void   setKernPA(float f){kernPA=f;};
 
  Type   getKernelPt(int i){return kernel[i];};
  Type  *getKernel(){return kernel;};

  int    getKernelWidth(){return kernWidth;};
  float  getStddevScale(){return stddevScale;};

  void   setBlankVal(Type b){blankVal = b;};
  Type   getBlankVal(){return blankVal;};

    bool isAllocated(){return allocated;};

private:
  float  kernMaj;      ///< The FWHM of the major axis of the elliptical Gaussian.
  float  kernMin;      ///< The FWHM of the minor axis of the elliptical Gaussian.
  float  kernPA;       ///< The position angle of the elliptical Gaussian.
  size_t kernWidth;    ///< The width of the kernel (in pixels).
  float  stddevScale;  ///< The factor by which the rms of the input array gets scaled by (assuming iid normally)
  Type  *kernel;       ///< The coefficients of the smoothing kernel
  bool   allocated;    ///< Have the coefficients been allocated in memory?
  Type   blankVal;     ///< What value to set blanks (when doing TRUNCATE mode)

};

#include <duchamp/Utils/GaussSmooth2D.tcc>

#endif  // GAUSSSMOOTH2D_H
 
