// -----------------------------------------------------------------------
// Statistics.hh: Definition of the StatsContainer class, that holds
// statistical parameters such as mean, median, rms, madfm.
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
#ifndef STATS_H
#define STATS_H

#include <iostream>
#include <fstream>
#include <math.h>

/// A namespace to control everything to do with statistical
/// calculations.

namespace Statistics
{

  /// @brief Divide by the following correction factor to convert from MADFM to sigma (rms) estimator. 
  const float correctionFactor = 0.6744888;

  /// @brief Multiply by the following correction factor to convert from trimmedSigma to sigma estimator. 
  const double trimToNormal = 1.17036753077;

  /// @brief A templated function to do the MADFM-to-rms conversion. 
  template <class T> float madfmToSigma(T madfm);
  /// @brief A non-templated function to do the rms-to-MADFM conversion. 
  float sigmaToMADFM(float sigma);


  /// @brief
  ///  Class to hold statistics for a given set of values.
  /// @details
  ///  This class is designed to hold all useful statistics for a given
  ///  array of numbers.  It does *not* hold the data themselves. It
  ///  provides the functions to calculate mean, rms, median and MADFM
  ///  (median absolute deviation from median), as well as functions to
  ///  control detection (ie. defining thresholds) in both standard
  ///  (sigma-clipping) cases and False Detection Rate scenarios.

  template <class Type> 
  class StatsContainer
  {
  public:
    StatsContainer();
    virtual ~StatsContainer(){};
    StatsContainer(const StatsContainer<Type>& s);
    StatsContainer<Type>& operator= (const StatsContainer<Type>& s);

    /// @brief A way of printing the statistics. 
    template <class T> 
    friend std::ostream& operator<< ( std::ostream& theStream, 
				      StatsContainer<T> &s);

    float getMean(){return mean;};
    void  setMean(float f){mean=f;};
    float getStddev(){return stddev;};
    void  setStddev(float f){stddev=f;};
    Type  getMedian(){return median;};
    void  setMedian(Type f){median=f;};
    Type  getMadfm(){return madfm;};
    void  setMadfm(Type f){madfm=f;};
    float getThreshold(){return threshold;};
    void  setThreshold(float f){threshold=f;};
    float getPThreshold(){return pThreshold;};
    void  setPThreshold(float f){pThreshold=f;};
    bool  getRobust(){return useRobust;};
    void  setRobust(bool b){useRobust=b;};
    bool  setUseFDR(){return useFDR;};
    void  setUseFDR(bool b){useFDR=b;};

    /// @brief Return the threshold as a signal-to-noise ratio. 
    float getThresholdSNR();

    /// @brief Set the threshold in units of a signal-to-noise ratio. 
    void  setThresholdSNR(float snr);

    /// @brief Convert a value to a signal-to-noise ratio. 
    float valueToSNR(float value);

    /// @brief Convert a signal-to-noise ratio to a flux value 
    float snrToValue(float snr);
    
    /// @brief Return the estimator of the middle value of the data. 
    float getMiddle();
    void  setMiddle(float middle);
    
    /// @brief Return the estimator of the amount of spread of the data.
    float getSpread();
    void  setSpread(float spread);

    /// @brief Scale the noise by a given factor. 
    void  scaleNoise(float scale);

    /// @brief Return the Gaussian probability of a value given the stats. 
    float getPValue(float value);

    /// @brief Is a value above the threshold? 
    bool isDetection(float value);

    /// @brief Set the comment characters
    void setCommentString(std::string comment){commentString = comment;};

    // Functions to calculate the stats for a given array.
    // The idea here is that there are two options to do the calculations:
    //   *The first just uses all the points in the array. If you need to 
    //     remove BLANK points (or something similar), do this beforehand.
    //   *Alternatively, construct a mask array of the same size,
    //     showing which points are good, and use the second option.

      /// @brief Directly set the statistics that have been calculated elsewhere.
      void define(float mean, Type median, float stddev, Type madfm);

    /// @brief Calculate statistics for all elements of a data array 
    void calculate(Type *array, long size);

    /// @brief Calculate statistics for a subset of a data array 
    void calculate(Type *array, long size, bool *mask);

    void writeToBinaryFile(std::string filename);
    std::streampos readFromBinaryFile(std::string filename, std::streampos loc=0);

  private:
    bool   defined;      // a flag indicating whether the stats are defined.

    float  mean;         ///< The mean, or average, of the values.
    float  stddev;       ///< The standard deviation, or R.M.S., or sigma...
    Type   median;       ///< The median of the values.
    Type   madfm;	 ///< The median absolute deviation from the median 

    float  threshold;    ///< a threshold for simple sigma-clipping
    float  pThreshold;   ///< a threshold for the FDR case -- the upper limit of P values that detected pixels can have.
    bool   useRobust;    ///< whether we use the two robust stats or not
    bool   useFDR;       ///< whether the FDR method is used for determining a detection

    std::string commentString; ///< Any comment characters etc that need to be prepended to any output via the << operator.

  };

}

#endif // STATS_H
