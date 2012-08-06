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

namespace duchamp
{

  /// @brief A class to store details of the wavelet's filter.
  /// @details This stores all details of and functions related to the
  ///  filter used to generate the wavelets used in the reconstruction
  ///  (and baseline subtraction) functions.
  /// 
  /// It stores the coefficients of the filter, as well as the number of scales
  /// associated with it, and the sigma factors, that relate the value of the rms
  /// at a given scale with the measured value.

  class Filter
  {
  public:
    // these are all in atrous.cc
    /// @brief Constructor
    Filter(); 
    /// @brief Copy Constructor
    Filter(const Filter& f);
    /// @brief Copy operator
    Filter& operator=(const Filter& f);
    /// @brief Destructor
    virtual ~Filter();                

    /// @brief Define the parameters for a given filter. 
    void   define(int code);

    /// @brief Calculate the number of scales possible with a given filter and data size. 
    unsigned int    getNumScales(size_t length);

    /// @brief Calculate the maximum number of pixels able to be analysed with a filter at a given scale. 
    unsigned int    getMaxSize(int scale);

    /// @brief Return the width of the filter 
    size_t    width(){return filter1D.size();};

    // these are inline functions.
    /// @brief Return the text name of the filter 
    std::string getName(){return name;};

    /// @brief Return the i-th value of the coefficient array. 
    double coeff(int i){return filter1D[i];};

    /// @brief Set the i-th value of the coefficient array. 
    void   setCoeff(int i, double val){filter1D[i] = val;};

    /// @brief Return the maximum number of scales in the sigmaFactor array for the given dimension 
    unsigned int    maxFactor(int dim){return maxNumScales[dim-1];};

    /// @brief Set the maximum number of scales in the sigmaFactor array for the given dimension 
    void   setMaxFactor(int dim, int val){maxNumScales[dim-1] = val;};

    /// @brief Return the sigma scaling factor for the given dimension and scale of the wavelet transform. 
    double sigmaFactor(int dim, int scale){return (*sigmaFactors[dim-1])[scale];};
    /// @brief Set the sigma scaling factor for the given dimension and scale of the wavelet transform. 
    void   setSigmaFactor(int dim, int scale, double val){(*sigmaFactors[dim])[scale] = val;};

  private:
    std::string name;                ///< what is the filter called?
    std::vector <double> filter1D;   ///< filter coefficients.
    std::vector <unsigned int> maxNumScales;  ///< max number of scales for the sigmaFactor arrays, for each dim.
    std::vector < std::vector <double>* > sigmaFactors; ///< arrays of sigmaFactors, one for each dim.

    // these are all in atrous.cc
    /// @brief Set up parameters for using the B3-Spline filter.
    void   loadSpline();  
    /// @brief Set up parameters for using the Triangle function.      
    void   loadTriangle();
    /// @brief Set up parameters for using the Haar wavelet.
    void   loadHaar();  

  };

}

#endif

