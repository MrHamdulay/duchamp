// -----------------------------------------------------------------------
// Hanning.hh: Definition of Hanning class, used to Hanning-smooth a
//             1D spectrum.
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
#ifndef HANNING_H
#define HANNING_H

/// @brief
///  Define a Hanning filter.
/// @details
///  A simple class to define a Hanning filter. It is characterised by
///  a full width \f$2a-1\f$ (this is the number of coefficients and a
///  set of coefficients that follow the functional form of \f$c(x) =
///  0.5 + 0.5\cos(\pi x / a)\f$.

class Hanning
{
public:
  Hanning();          ///< Basic constructor -- no filter width set.
  virtual ~Hanning(); ///< Destructor
  Hanning(const Hanning& h);
  Hanning& operator=(const Hanning& h);
  Hanning(int size);  ///< Specific constructor that sets width and coefficients

  void define(int size); ///< Define the size and the array of coefficients.

  float *smooth(float *array, int npts);  ///< Smooth an array with the Hanning filter.
    
private:
  int hanningSize; ///< The full width of the filter (number of coefficients)
  float *coeffs;   ///< The coefficients of the filter
  bool allocated;  ///< Have the coefficients been allocated in memory?

};

#endif  // HANNING_H
