#include <iostream>
#include <algorithm>
#include <math.h>
#include <Utils/utils.hh>

template <class T> T absval(T value)
{
  /**
   * Type-independent way of getting the absolute value.
   */
  if( value > 0) return value;
  else return 0-value;
}
template int absval<int>(int value);
template long absval<long>(long value);
template float absval<float>(float value);
template double absval<double>(double value);
//--------------------------------------------------------------------

template <class T> void findMinMax(const T *array, const int size, 
				   T &min, T &max)
{
  /**
   * A function to find the minimum and maximum values of a set of numbers.
   * \param array The array of data values. Type independent.
   * \param size The length of the array
   * \param min The returned value of the minimum value in the array.
   * \param max The returned value of the maximum value in the array.
   */
  min = max = array[0];
  for(int i=1;i<size;i++) {
    if(array[i]<min) min=array[i];
    if(array[i]>max) max=array[i];
  }
}
template void findMinMax<int>(const int *array, const int size, 
				 int &min, int &max);
template void findMinMax<long>(const long *array, const int size, 
				  long &min, long &max);
template void findMinMax<float>(const float *array, const int size, 
				   float &min, float &max);
template void findMinMax<double>(const double *array, const int size, 
				    double &min, double &max);
//--------------------------------------------------------------------

template <class T> float findMean(T *array, int size)
{
  /**
   * Find the mean of an array of numbers. Type independent.
   * \param array The array of numbers.
   * \param size The length of the array.
   * \return The mean value of the array, returned as a float
   */
  float mean = array[0];
  for(int i=1;i<size;i++) mean += array[i];
  mean /= float(size);
  return mean;
}
template float findMean<int>(int *array, int size);
template float findMean<long>(long *array, int size);
template float findMean<float>(float *array, int size);
template float findMean<double>(double *array, int size);
//--------------------------------------------------------------------

template <class T> float findStddev(T *array, int size)
{
  /**
   * Find the rms or standard deviation of an array of numbers. Type independent.
   * \param array The array of numbers.
   * \param size The length of the array.
   * \return The rms value of the array, returned as a float
   */
  float mean = findMean(array,size);
  float stddev = (array[0]-mean) * (array[0]-mean);
  for(int i=1;i<size;i++) stddev += (array[i]-mean)*(array[i]-mean);
  stddev = sqrt(stddev/float(size-1));
  return stddev;
}
template float findStddev<int>(int *array, int size);
template float findStddev<long>(long *array, int size);
template float findStddev<float>(float *array, int size);
template float findStddev<double>(double *array, int size);
//--------------------------------------------------------------------

template <class T> T findMedian(T *array, int size)
{
  /**
   * Find the median value of an array of numbers. Type independent.
   * \param array The array of numbers.
   * \param size The length of the array.
   * \return The median value of the array, returned as the same type as the array.
   */
  T *newarray = new T[size];
  T median;
  for(int i=0;i<size;i++) newarray[i] = array[i];
  std::sort(newarray,newarray+size);
  if((size%2)==0) median = (newarray[size/2-1]+newarray[size/2])/2;
  else median = newarray[size/2];
  delete [] newarray;
  return median;
}
template int findMedian<int>(int *array, int size);
template long findMedian<long>(long *array, int size);
template float findMedian<float>(float *array, int size);
template double findMedian<double>(double *array, int size);
//--------------------------------------------------------------------

template <class T> T findMADFM(T *array, int size)
{
  /**
   * Find the median absolute deviation from the median value of an
   * array of numbers. Type independent.
   *
   * \param array The array of numbers.
   * \param size The length of the array.
   * \return The median absolute deviation from the median value of
   * the array, returned as the same type as the array.
   */
  T *newarray = new T[size];
  T median = findMedian<T>(array,size);
  T madfm;
  for(int i=0;i<size;i++) newarray[i] = absval(array[i]-median);
  std::sort(newarray,newarray+size);
  if((size%2)==0) madfm = (newarray[size/2-1]+newarray[size/2])/2;
  else madfm = newarray[size/2];
  delete [] newarray;
  return madfm;
}
template int findMADFM<int>(int *array, int size);
template long findMADFM<long>(long *array, int size);
template float findMADFM<float>(float *array, int size);
template double findMADFM<double>(double *array, int size);
//--------------------------------------------------------------------

template <class T> void findMedianStats(T *array, int size, 
					T &median, T &madfm)
{
  /**
   * Find the median and the median absolute deviation from the median
   * value of an array of numbers. Type independent.
   *
   * \param array The array of numbers.
   * \param size The length of the array.
   * \param median The median value of the array, returned as the same type as the array.
   * \param madfm The median absolute deviation from the median value of
   * the array, returned as the same type as the array.
   */
  if(size==0){
    std::cerr << "Error in findMedianStats: zero sized array!\n";
    return;
  }
  T *newarray = new T[size];

  for(int i=0;i<size;i++) newarray[i] = array[i];
  std::sort(newarray,newarray+size);
  if((size%2)==0) median = (newarray[size/2-1]+newarray[size/2])/2;
  else median = newarray[size/2];

  for(int i=0;i<size;i++) newarray[i] = absval(array[i]-median);
  std::sort(newarray,newarray+size);
  if((size%2)==0) madfm = (newarray[size/2-1]+newarray[size/2])/2;
  else madfm = newarray[size/2];

  delete [] newarray;
}
template void findMedianStats<int>(int *array, int size, 
				      int &median, int &madfm);
template void findMedianStats<long>(long *array, int size, 
				       long &median, long &madfm);
template void findMedianStats<float>(float *array, int size, 
					float &median, float &madfm);
template void findMedianStats<double>(double *array, int size, 
					 double &median, double &madfm);
//--------------------------------------------------------------------

template <class T> void findMedianStats(T *array, int size, bool *isGood, 
					T &median, T &madfm)
{
  /**
   * Find the median and the median absolute deviation from the median
   * value of a subset of an array of numbers. The subset is defined
   * by an array of bool variables. Type independent.
   *
   * \param array The array of numbers.
   * \param size The length of the array.
   * \param isGood An array of the same length that says whether to
   * include each member of the array in the calculations.
   * \param median The median value of the array, returned as the same type as the array.
   * \param madfm The median absolute deviation from the median value of
   * the array, returned as the same type as the array.
   */
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findMedianStats: no good values!\n";
    return;
  }
  T *newarray = new T[goodSize];
  for(int i=0;i<size;i++) if(isGood[i]) newarray[goodSize++] = array[i];
  std::sort(newarray,newarray+goodSize);
  if((goodSize%2)==0) 
    median = (newarray[goodSize/2-1]+newarray[goodSize/2])/2;
  else 
    median = newarray[goodSize/2];

  for(int i=0;i<goodSize;i++) newarray[i] = absval(newarray[i]-median);
  std::sort(newarray,newarray+goodSize);
  if((goodSize%2)==0) 
    madfm = (newarray[goodSize/2-1]+newarray[goodSize/2])/2;
  else 
    madfm = newarray[goodSize/2];

  delete [] newarray;
}
template void findMedianStats<int>(int *array, int size, bool *isGood, 
				      int &median, int &madfm);
template void findMedianStats<long>(long *array, int size, bool *isGood, 
				       long &median, long &madfm);
template void findMedianStats<float>(float *array, int size, bool *isGood, 
					float &median, float &madfm);
template void findMedianStats<double>(double *array, int size, bool *isGood, 
					 double &median, double &madfm);
//--------------------------------------------------------------------
  

template <class T> void findNormalStats(T *array, int size, 
					float &mean, float &stddev)
{
  /**
   * Find the mean and rms or standard deviation of an array of numbers. Type independent.
   *
   * \param array The array of numbers.
   * \param size The length of the array.
   * \param mean The mean value of the array, returned as a float.
   * \param stddev The rms or standard deviation of the array, returned as a float.
   */
  if(size==0){
    std::cerr << "Error in findNormalStats: zero sized array!\n";
    return;
  }
  mean = array[0];
  for(int i=1;i<size;i++){
    mean += array[i];
  }
  mean /= float(size);

  stddev = (array[0]-mean) * (array[0]-mean);
  for(int i=1;i<size;i++){
    float sqdiff = (array[i]-mean)*(array[i]-mean);
    stddev += sqdiff;
  }
  stddev = sqrt(stddev/float(size-1));

}
template void findNormalStats<int>(int *array, int size, 
				      float &mean, float &stddev);
template void findNormalStats<long>(long *array, int size, 
				       float &mean, float &stddev);
template void findNormalStats<float>(float *array, int size, 
					float &mean, float &stddev);
template void findNormalStats<double>(double *array, int size, 
					 float &mean, float &stddev);
//--------------------------------------------------------------------

template <class T> void findNormalStats(T *array, int size, bool *isGood, 
					float &mean, float &stddev)
{
  /**
   * Find the mean and rms or standard deviation of a subset of an
   * array of numbers. The subset is defined by an array of bool
   * variables. Type independent.
   *
   * \param array The array of numbers.
   * \param size The length of the array.
   * \param isGood An array of the same length that says whether to
   * include each member of the array in the calculations.
   * \param mean The mean value of the array, returned as a float.
   * \param stddev The rms or standard deviation of the array, returned as a float.
   */
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findNormalStats: no good values!\n";
    return;
  }
  int start=0;
  while(!isGood[start]){start++;}
  mean = array[start];
  for(int i=start+1;i<size;i++){
    if(isGood[i]) mean += array[i];
  }
  mean /= float(goodSize);

  stddev = (array[start]-mean) * (array[start]-mean);
  for(int i=1;i<size;i++){
    if(isGood[i]) stddev += (array[i]-mean)*(array[i]-mean);
  }
  stddev = sqrt(stddev/float(goodSize-1));

}
template void findNormalStats<int>(int *array, int size, bool *isGood, 
				      float &mean, float &stddev);
template void findNormalStats<long>(long *array, int size, bool *isGood, 
				       float &mean, float &stddev);
template void findNormalStats<float>(float *array, int size, bool *isGood, 
					float &mean, float &stddev);
template void findNormalStats<double>(double *array, int size, bool *isGood, 
					 float &mean, float &stddev);
//--------------------------------------------------------------------
