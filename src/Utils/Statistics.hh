#ifndef STATS_H
#define STATS_H

#include <iostream>
#include <math.h>

/**
 * A namespace to control everything to do with statistical
 * calculations.
 */

namespace Statistics
{

  /** Divide by the following correction factor to convert from MADFM
      to sigma (rms) estimator. */
  const float correctionFactor = 0.6744888;

  /** Multiply by the following correction factor to convert from
      trimmedSigma to sigma estimator. */
  const double trimToNormal = 1.17036753077;

  /** A templated function to do the MADFM-to-rms conversion. */
  template <class T> float madfmToSigma(T madfm);
  /** A templated function to do the rms-to-MADFM conversion. */
  template <class T> float sigmaToMADFM(T sigma);


  /**
   *  Class to hold statistics for a given set of values.
   *
   *  This class is designed to hold all useful statistics for a given
   *  array of numbers.  It does *not* hold the data themselves. It
   *  provides the functions to calculate mean, rms, median and MADFM
   *  (median absolute deviation from median), as well as functions to
   *  control detection (ie. defining thresholds) in both standard
   *  (sigma-clipping) cases and False Detection Rate scenarios.
   */

  template <class Type> 
  class StatsContainer
  {
  public:
    StatsContainer(){useRobust=true; defined=false; useFDR=false;};
    virtual ~StatsContainer(){};
    StatsContainer(const StatsContainer<Type>& s);
    StatsContainer<Type>& operator= (const StatsContainer<Type>& s);

    /** A way of printing the statistics. */
    template <class T> 
    friend std::ostream& operator<< ( std::ostream& theStream, StatsContainer<T> &s);

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

    /** Return the threshold as a signal-to-noise ratio. */
    float getThresholdSNR();

    /** Set the threshold in units of a signal-to-noise ratio. */
    void  setThresholdSNR(float snr);

    /** Convert a value to a signal-to-noise ratio. */
    float getSNR(float value);
    
    /** Return the estimator of the middle value of the data. */
    float getMiddle();
    
    /** Return the estimator of the amount of spread of the data.*/
    float getSpread();

    /** Scale the noise by a given factor .*/
    void  scaleNoise(float scale);

    /** Return the Gaussian probability of a value given the stats. */
    float getPValue(float value);

    /** Is a value above the threshold? */
    bool isDetection(float value);

    // Functions to calculate the stats for a given array.
    // The idea here is that there are two options to do the calculations:
    //   *The first just uses all the points in the array. If you need to 
    //     remove BLANK points (or something similar), do this beforehand.
    //   *Alternatively, construct a bool array of the same size, showing which
    //     points are good, and use the second option.

    /** Calculate statistics for all elements of a data array */
    void calculate(Type *array, long size);

    /** Calculate statistics for a subset of a data array */
    void calculate(Type *array, long size, bool *isGood);

  private:
    bool   defined;      // a flag indicating whether the stats are defined.

    float  mean;         ///< The mean, or average, of the values.
    float  stddev;       ///< The standard deviation, or R.M.S., or sigma...
    Type   median;       ///< The median of the values.
    Type   madfm;	 ///< The median absolute deviation from the median 

    float  threshold;    ///< a threshold for simple sigma-clipping
    float  pThreshold;   ///< a threshold for the FDR case -- the
			 ///   upper limit of P values that detected
			 ///   pixels can have.
    bool   useRobust;    ///< whether we use the two robust stats or not
    bool   useFDR;       ///< whether the FDR method is used for
			 ///   determining a detection

  };

}

#endif /*STATS_H*/
