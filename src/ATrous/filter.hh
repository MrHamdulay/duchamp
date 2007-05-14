// -----------------------------------------------------------------------
// filter.hh: Defining a filter function for wavelet reconstruction.
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
#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <string>

/**
 * A class to store details of the wavelet's filter.
 * This stores all details of and functions related to the filter used to
 *  generate the wavelets used in the reconstruction (and baseline
 *  subtraction) functions.
 * 
 * It stores the coefficients of the filter, as well as the number of scales
 * associated with it, and the sigma factors, that relate the value of the rms
 * at a given scale with the measured value.
 */

class Filter
{
public:
  // these are all in atrous.cc
  Filter();                         ///< Constructor
  virtual ~Filter();                ///< Destructor

  /** Define the parameters for a given filter. */
  void   define(int code);

  /** Calculate the number of scales possible with a given filter and
      data size. */
  int    getNumScales(long length);

  /** Calculate the maximum number of pixels able to be analysed with
      a filter at a given scale. */
  int    getMaxSize(int scale);

  /** Return the width of the filter */
  int    width(){return filter1D.size();};

  // these are inline functions.
  /** Return the text name of the filter */
  std::string getName(){return name;};

  /** Return the i-th value of the coefficient array. */
  double coeff(int i){return filter1D[i];};

  /** Set the i-th value of the coefficient array. */
  void   setCoeff(int i, double val){filter1D[i] = val;};

  /** Return the maximum number of scales in the sigmaFactor array for
      the given dimension */
  int    maxFactor(int dim){return maxNumScales[dim-1];};

  /** Set the maximum number of scales in the sigmaFactor array for
      the given dimension */
  void   setMaxFactor(int dim, int val){maxNumScales[dim-1] = val;};

  /** Return the sigma scaling factor for the given dimension and
      scale of the wavelet transform. */
  double sigmaFactor(int dim, int scale){return (*sigmaFactors[dim-1])[scale];};
  /** Set the sigma scaling factor for the given dimension and
      scale of the wavelet transform. */
  void   setSigmaFactor(int dim, int scale, double val){(*sigmaFactors[dim])[scale] = val;};

private:
  std::string name;                ///< what is the filter called?
  std::vector <double> filter1D;   ///< filter coefficients.
  std::vector <int> maxNumScales;  ///< max number of scales for the sigmaFactor arrays, for each dim.
  std::vector < std::vector <double>* > sigmaFactors; ///< arrays of sigmaFactors, one for each dim.

  // these are all in atrous.cc
  void   loadSpline();        ///< set up parameters for using the
			      ///   B3-Spline filter.
  void   loadTriangle();      ///< set up parameters for using the
			      ///   Triangle function.
  void   loadHaar();          ///< set up parameters for using the
			      ///   Haar wavelet.

};

#endif

