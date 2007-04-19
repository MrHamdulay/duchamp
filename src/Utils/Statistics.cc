#include <iostream>
#include <Utils/Statistics.hh>
#include <Utils/utils.hh>

namespace Statistics
{

  template <class T> 
  float madfmToSigma(T madfm){
    return float(madfm)/correctionFactor;
  };
  template float madfmToSigma<int>(int madfm);
  template float madfmToSigma<long>(long madfm);
  template float madfmToSigma<float>(float madfm);
  template float madfmToSigma<double>(double madfm);

  //--------------------------------------------------------------------
  //--------------------------------------------------------------------

  template <class Type> 
  StatsContainer<Type>::StatsContainer(const StatsContainer<Type>& s)
  {
    /** 
     *  The copy constructor for the StatsContainer class.
     */

    this->defined    = s.defined;
    this->mean       = s.mean;
    this->stddev     = s.stddev;
    this->median     = s.median;
    this->madfm      = s.madfm;
    this->threshold  = s.threshold;
    this->pThreshold = s.pThreshold;
    this->useRobust  = s.useRobust;
    this->useFDR     = s.useFDR;
  }
  template StatsContainer<int>::StatsContainer(const StatsContainer<int>& s);
  template StatsContainer<long>::StatsContainer(const StatsContainer<long>& s);
  template StatsContainer<float>::StatsContainer(const StatsContainer<float>& s);
  template StatsContainer<double>::StatsContainer(const StatsContainer<double>& s);
  //--------------------------------------------------------------------

  template <class Type> 
  StatsContainer<Type>& 
  StatsContainer<Type>::operator= (const StatsContainer<Type>& s)
  {
    /** 
     *  The assignment operator for the StatsContainer class.
     */

    if(this == &s) return *this;
    this->defined    = s.defined;
    this->mean       = s.mean;
    this->stddev     = s.stddev;
    this->median     = s.median;
    this->madfm      = s.madfm;
    this->threshold  = s.threshold;
    this->pThreshold = s.pThreshold;
    this->useRobust  = s.useRobust;
    this->useFDR     = s.useFDR;
    return *this;
  }
  template StatsContainer<int>& StatsContainer<int>::operator= (const StatsContainer<int>& s);
  template StatsContainer<long>& StatsContainer<long>::operator= (const StatsContainer<long>& s);
  template StatsContainer<float>& StatsContainer<float>::operator= (const StatsContainer<float>& s);
  template StatsContainer<double>& StatsContainer<double>::operator= (const StatsContainer<double>& s);
  //--------------------------------------------------------------------

  template <class Type> 
  float StatsContainer<Type>::getThresholdSNR()
  {
    /** 
     * The SNR is defined in terms of excess over the middle estimator
     * in units of the spread estimator.
    */
    return (threshold - this->getMiddle())/this->getSpread();
  }
  template float StatsContainer<int>::getThresholdSNR();
  template float StatsContainer<long>::getThresholdSNR();
  template float StatsContainer<float>::getThresholdSNR();
  template float StatsContainer<double>::getThresholdSNR();
  //--------------------------------------------------------------------

  template <class Type> 
  void  StatsContainer<Type>::setThresholdSNR(float snr)
  {
    /** 
     * The SNR is defined in terms of excess over the middle estimator
     * in units of the spread estimator.
     */
    threshold=this->getMiddle() + snr*this->getSpread();
  }
  template void StatsContainer<int>::setThresholdSNR(float snr);
  template void StatsContainer<long>::setThresholdSNR(float snr);
  template void StatsContainer<float>::setThresholdSNR(float snr);
  template void StatsContainer<double>::setThresholdSNR(float snr);
  //--------------------------------------------------------------------
  
  template <class Type> 
  float StatsContainer<Type>::getSNR(float value)
  {
    /** 
     * The SNR is defined in terms of excess over the middle estimator
     * in units of the spread estimator.
     */
    return (value - this->getMiddle())/this->getSpread();
  }
  template float StatsContainer<int>::getSNR(float value);
  template float StatsContainer<long>::getSNR(float value);
  template float StatsContainer<float>::getSNR(float value);
  template float StatsContainer<double>::getSNR(float value);
  //--------------------------------------------------------------------
    
  template <class Type> 
  float StatsContainer<Type>::getMiddle()
  {
    /** 
     * The middle value is determined by the StatsContainer::useRobust
     * flag -- it will be either the median (if true), or the mean (if
     * false).
     */
    if(useRobust) return float(median); 
    else return mean;
  }
  template float StatsContainer<int>::getMiddle();
  template float StatsContainer<long>::getMiddle();
  template float StatsContainer<float>::getMiddle();
  template float StatsContainer<double>::getMiddle();
  //--------------------------------------------------------------------
    
  template <class Type> 
  float StatsContainer<Type>::getSpread(){
    /** 
     * The spread value returned is determined by the
     * StatsContainer::useRobust flag -- it will be either the madfm
     * (if true), or the rms (if false). If robust, the madfm will be
     * converted to an equivalent rms under the assumption of
     * Gaussianity, using the Statistics::madfmToSigma function.
     */
    if(useRobust) return madfmToSigma(madfm); 
    else return stddev;
  }
  template float StatsContainer<int>::getSpread();
  template float StatsContainer<long>::getSpread();
  template float StatsContainer<float>::getSpread();
  template float StatsContainer<double>::getSpread();
  //--------------------------------------------------------------------
    
  template <class Type> 
  float StatsContainer<Type>::getPValue(float value)
  {
    /** 
     * Get the "probability", under the assumption of normality, of a
     * value occuring.  
     *
     * It is defined by \f$0.5 \operatorname{erfc}(z/\sqrt{2})\f$, where
     * \f$z=(x-\mu)/\sigma\f$. We need the factor of 0.5 here, as we are
     * only considering the positive tail of the distribution -- we
     * don't care about negative detections.
     */
    
    float zStat = (value - this->getMiddle()) / this->getSpread();
    return 0.5 * erfc( zStat / M_SQRT2 );
  }
  template float StatsContainer<int>::getPValue(float value);
  template float StatsContainer<long>::getPValue(float value);
  template float StatsContainer<float>::getPValue(float value);
  template float StatsContainer<double>::getPValue(float value);
  //--------------------------------------------------------------------
    
  template <class Type> 
  bool StatsContainer<Type>::isDetection(float value){
    /** 
     * Compares the value given to the correct threshold, depending on
     * the value of the StatsContainer::useFDR flag. 
     */
    if(useFDR) return (this->getPValue(value) < this->pThreshold);
    else       return (value > this->threshold);
  }
  template bool StatsContainer<int>::isDetection(float value);
  template bool StatsContainer<long>::isDetection(float value);
  template bool StatsContainer<float>::isDetection(float value);
  template bool StatsContainer<double>::isDetection(float value);
  //--------------------------------------------------------------------

  template <class Type> 
  void StatsContainer<Type>::calculate(Type *array, long size)
  {
    /**
     * Calculate all four statistics for all elements of a given
     * array.
     *
     * \param array The input data array.
     * \param size The length of the input array
     */
    findNormalStats(array, size, this->mean, this->stddev);
    findMedianStats(array, size, this->median, this->madfm);
    this->defined = true;
  }
  template void StatsContainer<int>::calculate(int *array, long size);
  template void StatsContainer<long>::calculate(long *array, long size);
  template void StatsContainer<float>::calculate(float *array, long size);
  template void StatsContainer<double>::calculate(double *array, long size);
  //--------------------------------------------------------------------

  template <class Type> 
  void StatsContainer<Type>::calculate(Type *array, long size, bool *isGood)
  {
    /**
     * Calculate all four statistics for a subset of a given
     * array. The subset is defined by an array of bool 
     * variables.  
     *
     * \param array The input data array.
     * \param size The length of the input array
     * \param isGood An array of the same length that says whether to
     * include each member of the array in the calculations.
     */
    findNormalStats(array, size, isGood, this->mean, this->stddev);
    findMedianStats(array, size, isGood, this->median, this->madfm);
    this->defined = true;
  }
  template void StatsContainer<int>::calculate(int *array, long size, bool *isGood);
  template void StatsContainer<long>::calculate(long *array, long size, bool *isGood);
  template void StatsContainer<float>::calculate(float *array, long size, bool *isGood);
  template void StatsContainer<double>::calculate(double *array, long size, bool *isGood);
  //--------------------------------------------------------------------

  template <class Type> 
  std::ostream& operator<< (std::ostream& theStream, StatsContainer<Type> &s)
  {
    /**
     * Prints out the four key statistics to the requested stream.
     */
    theStream << "Mean   = "   << s.mean   << "\t"
	      << "Std.Dev. = " << s.stddev << "\n"
	      << "Median = "   << s.median << "\t"
	      << "MADFM    = " << s.madfm  << "\n";
    return theStream;
  }
  template std::ostream& operator<<<int> (std::ostream& theStream, StatsContainer<int> &s);
  template std::ostream& operator<<<long> (std::ostream& theStream, StatsContainer<long> &s);
  template std::ostream& operator<<<float> (std::ostream& theStream, StatsContainer<float> &s);
  template std::ostream& operator<<<double> (std::ostream& theStream, StatsContainer<double> &s);



}  // matches:  namespace Statistics {
