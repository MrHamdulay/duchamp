#ifndef STATS_H
#define STATS_H

#include <iostream>
#include <math.h>
#ifndef UTILS_H
#include <Utils/utils.hh>
#endif

/**
 * A namespace to control everything to do with statistical calculations.
 */

namespace Statistics
{

  /** Divide by the following correction factor to convert from MADFM to sigma (rms) estimator. */
  const float correctionFactor = 0.6744888;

  /** Multiply by the following correction factor to convert from trimmedSigma to sigma estimator. */
  const double trimToNormal = 1.17036753077;

  /** A templated function to do the MADFM-to-rms conversion. */
  template <class T> float madfmToSigma(T madfm){
    return float(madfm)/correctionFactor;
  };
  template float madfmToSigma<int>(int madfm);
  template float madfmToSigma<long>(long madfm);
  template float madfmToSigma<float>(float madfm);
  template float madfmToSigma<double>(double madfm);

  /**
   *  Class to hold statistics for a given set.
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

    /** A way of printing the statistics and thresholds. */
    template <class T> friend std::ostream& operator<< ( std::ostream& theStream, StatsContainer<T> &s);

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
    float getThresholdSNR(){
      return (threshold - this->getMiddle())/this->getSpread();};
    void  setThresholdSNR(float snr){
      threshold=this->getMiddle() + snr*this->getSpread();};
    float getPThreshold(){return pThreshold;};
    void  setPThreshold(float f){pThreshold=f;};
    bool  getRobust(){return useRobust;};
    void  setRobust(bool b){useRobust=b;};
    bool  setUseFDR(){return useFDR;};
    void  setUseFDR(bool b){useFDR=b;};

    float getMiddle(){if(useRobust) return float(median); else return mean;};
    float getSpread(){
      if(useRobust) return madfmToSigma(madfm); 
      else return stddev;
    };

    /** Get the "probability", under the assumption of normality, of a value. */
    float getPValue(float value){
      float zStat = (value - this->getMiddle()) / this->getSpread();
      return 0.5 * erfc( zStat / M_SQRT2 );
    };

    /** Is a value above the threshold? */
    bool isDetection(float value){
      if(useFDR) return (this->getPValue(value) < this->pThreshold);
      else       return (value > this->threshold);
    };

    // Functions to calculate the stats for a given array.
    // The idea here is that there are two options to do the calculations:
    //   *The first just uses all the points in the array. If you need to 
    //     remove BLANK points (or something similar), do this beforehand.
    //   *Alternatively, construct a bool array of the same size, showing which
    //     points are good, and use the second option.
    void calculate(Type *array, long size);
    void calculate(Type *array, long size, bool *isGood);

  private:
    bool   defined;      // a flag indicating whether the stats are defined.

    // basic statistics
    float  mean;    
    float  stddev;       ///< The standard deviation, or R.M.S., or sigma...
    Type   median; 
    Type   madfm;	 ///< The median absolute deviation from the median 

    float  threshold;    ///< a threshold for simple sigma-clipping
    float  pThreshold;   ///< a threshold for the FDR case -- the upper limit of P values that detected pixels can have.
    bool   useRobust;    ///< whether we use the two robust stats or not
    bool   useFDR;       ///< whether the FDR method is used for determining a detection

  };

}

#endif /*STATS_H*/
