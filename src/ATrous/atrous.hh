// -----------------------------------------------------------------------
// atrous.hh: Definitions for wavelet reconstruction.
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
#ifndef ATROUS_H
#define ATROUS_H

namespace duchamp
{

  class Param;

  const float reconTolerance = 0.005; ///< The tolerance in the reconstruction.


  /////////////////////////////////////////////////////////////////////////

  /// @brief Perform a 1-dimensional a trous wavelet reconstruction. 
  void atrous1DReconstruct(size_t &size, float *&input, 
			   float *&output, Param &par);

  /// @brief Perform a 2-dimensional a trous wavelet reconstruction. 
  void atrous2DReconstruct(size_t &xdim, size_t &ydim, float *&input,
			   float *&output, Param &par);

  /// @brief Perform a 3-dimensional a trous wavelet reconstruction. 
  void atrous3DReconstruct(size_t &xdim, size_t &ydim, size_t &zdim, 
			   float *&input,float *&output, Param &par);

  /// @brief Subtract a baseline from a set of spectra in a cube. 
  void baselineSubtract(size_t numSpec, size_t specLength, 
			float *originalCube, float *baseline, Param &par);

  /// @brief Find the baseline of a 1-D spectrum. 
  void findBaseline(size_t size, float *input, float *baseline, Param &par);

  /// @brief Find the baseline of a 1-D spectrum. 
  void findBaseline(size_t size, float *input, float *baseline);

}

#endif
