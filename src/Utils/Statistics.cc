#include <Utils/Statistics.hh>
#include <Utils/utils.hh>
using namespace Statistics;

template <class T> 
void Statistics::StatsContainer<T>::calculate(T *array, long size)
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

