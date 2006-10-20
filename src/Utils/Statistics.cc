#include <Utils/Statistics.hh>
#include <Utils/utils.hh>
using namespace Statistics;

template <class T> Statistics::StatsContainer<T>::StatsContainer(const StatsContainer<T>& s)
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

template <class T> Statistics::StatsContainer<T>& StatsContainer<T>::operator= (const StatsContainer<T>& s)
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

template <class T> void Statistics::StatsContainer<T>::calculate(T *array, long size)
{
  findNormalStats(array, size, this->mean, this->stddev);
  findMedianStats(array, size, this->median, this->madfm);
  this->defined = true;
}
template void Statistics::StatsContainer<int>::calculate(int *array, long size);
template void Statistics::StatsContainer<long>::calculate(long *array, long size);
template void Statistics::StatsContainer<float>::calculate(float *array, long size);
template void Statistics::StatsContainer<double>::calculate(double *array, long size);
//--------------------------------------------------------------------

template <class T> 
void Statistics::StatsContainer<T>::calculate(T *array, long size, bool *isGood)
{
  findNormalStats(array, size, isGood, this->mean, this->stddev);
  findMedianStats(array, size, isGood, this->median, this->madfm);
  this->defined = true;
}
template void Statistics::StatsContainer<int>::calculate(int *array, long size, bool *isGood);
template void Statistics::StatsContainer<long>::calculate(long *array, long size, bool *isGood);
template void Statistics::StatsContainer<float>::calculate(float *array, long size, bool *isGood);
template void Statistics::StatsContainer<double>::calculate(double *array, long size, bool *isGood);

