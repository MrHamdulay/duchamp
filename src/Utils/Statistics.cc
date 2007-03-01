#include <iostream>
#include <Utils/Statistics.hh>
#include <Utils/utils.hh>

namespace Statistics
{
//    template StatsContainer<int>;
//    template StatsContainer<long>;
//    template StatsContainer<float>;
//    template StatsContainer<double>;
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------

  template <class Type> 
  StatsContainer<Type>::StatsContainer(const StatsContainer<Type>& s)
  {
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
  StatsContainer<Type>& StatsContainer<Type>::operator= (const StatsContainer<Type>& s)
  {
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
  template StatsContainer<int>& StatsContainer<int>::operator= (const StatsContainer<int>& s);
  template StatsContainer<long>& StatsContainer<long>::operator= (const StatsContainer<long>& s);
  template StatsContainer<float>& StatsContainer<float>::operator= (const StatsContainer<float>& s);
  template StatsContainer<double>& StatsContainer<double>::operator= (const StatsContainer<double>& s);
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
  }
  template std::ostream& operator<<<int> (std::ostream& theStream, StatsContainer<int> &s);
  template std::ostream& operator<<<long> (std::ostream& theStream, StatsContainer<long> &s);
  template std::ostream& operator<<<float> (std::ostream& theStream, StatsContainer<float> &s);
  template std::ostream& operator<<<double> (std::ostream& theStream, StatsContainer<double> &s);



}  // matches:  namespace Statistics {
