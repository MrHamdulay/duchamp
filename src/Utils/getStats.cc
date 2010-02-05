// -----------------------------------------------------------------------
// getStats.cc: Basic functions to calculate statistical parameters
//              such as mean, median, rms, madfm, min, max.
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
#include <iostream>
#include <algorithm>
#include <math.h>
#include <duchamp/Utils/utils.hh>

template <class T> T absval(T value)
{
  /// Type-independent way of getting the absolute value.
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
  /// @details
  /// A function to find the minimum and maximum values of a set of numbers.
  /// \param array The array of data values. Type independent.
  /// \param size The length of the array
  /// \param min The returned value of the minimum value in the array.
  /// \param max The returned value of the maximum value in the array.
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
  /// @details
  /// Find the mean of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \return The mean value of the array, returned as a float
  double mean = double(array[0]);
  for(int i=1;i<size;i++) mean += double(array[i]);
  mean /= double(size);
  return float(mean);
}
template float findMean<int>(int *array, int size);
template float findMean<long>(long *array, int size);
template float findMean<float>(float *array, int size);
template float findMean<double>(double *array, int size);
//--------------------------------------------------------------------

template <class T> float findStddev(T *array, int size)
{
  /// @details
  /// Find the rms or standard deviation of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \return The rms value of the array, returned as a float
  double mean = double(findMean(array,size));
  double stddev = (double(array[0])-mean) * (double(array[0])-mean);
  for(int i=1;i<size;i++) stddev += (double(array[i])-mean)*(double(array[i])-mean);
  stddev = sqrt(stddev/double(size-1));
  return float(stddev);
}
template float findStddev<int>(int *array, int size);
template float findStddev<long>(long *array, int size);
template float findStddev<float>(float *array, int size);
template float findStddev<double>(double *array, int size);
//--------------------------------------------------------------------

template <class T> T findMedian(T *array, int size, bool changeArray)
{
  /// @details
  /// Find the median value of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered (ie. the order of elements will be changed).
  /// \return The median value of the array, returned as the same type as the array.
  T *newarray;
  if(changeArray) newarray = array;
  else{
    newarray = new T[size];
    for(int i=0;i<size;i++) newarray[i] = array[i];
  }
  T median;
  bool isOdd = ((size/2)!=0);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  median = newarray[size/2];
  if(isOdd){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    median += newarray[size/2-1];
    median /= 2.;
  }
  if(!changeArray) delete [] newarray;
  return median;
}
template int findMedian<int>(int *array, int size, bool changeArray);
template long findMedian<long>(long *array, int size, bool changeArray);
template float findMedian<float>(float *array, int size, bool changeArray);
template double findMedian<double>(double *array, int size, bool changeArray);
//--------------------------------------------------------------------

template <class T> T findMADFM(T *array, int size, bool changeArray)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  T *newarray;
  if(changeArray) newarray = array;
  else newarray = new T[size];

  T median = findMedian<T>(array,size,changeArray);
  T madfm;
  bool isOdd = ((size/2)!=0);
  for(int i=0;i<size;i++) newarray[i] = absval(array[i]-median);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  madfm = newarray[size/2];
  if(isOdd){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    madfm += newarray[size/2-1];
    madfm /= 2.;
  }
  if(!changeArray) delete [] newarray;
  return madfm;
}
template int findMADFM<int>(int *array, int size, bool changeArray);
template long findMADFM<long>(long *array, int size, bool changeArray);
template float findMADFM<float>(float *array, int size, bool changeArray);
template double findMADFM<double>(double *array, int size, bool changeArray);
//--------------------------------------------------------------------

template <class T> void findMedianStats(T *array, int size, 
					T &median, T &madfm)
{
  /// @details
  /// Find the median and the median absolute deviation from the median
  /// value of an array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.
  if(size==0){
    std::cerr << "Error in findMedianStats: zero sized array!\n";
    return;
  }
  T *newarray = new T[size];
   for(int i=0;i<size;i++) newarray[i] = array[i];

   median = findMedian(newarray,size,true);
   madfm = findMADFM(newarray,size,true);

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

template <class T> void findMedianStats(T *array, int size, bool *mask, 
					T &median, T &madfm)
{
  /// @details
  /// Find the median and the median absolute deviation from the median
  /// value of a subset of an array of numbers. The subset is defined
  /// by an array of bool variables. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only use
  /// values where mask=true.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.

  int goodSize=0;
  for(int i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findMedianStats: no good values!\n";
    return;
  }
  T *newarray = new T[goodSize];

  goodSize=0;
  for(int i=0;i<size;i++) if(mask[i]) newarray[goodSize++] = array[i];
  median = findMedian(newarray,goodSize,true);
  madfm = findMADFM(newarray,goodSize,true);

  delete [] newarray;
}
template void findMedianStats<int>(int *array, int size, bool *mask, 
				      int &median, int &madfm);
template void findMedianStats<long>(long *array, int size, bool *mask, 
				       long &median, long &madfm);
template void findMedianStats<float>(float *array, int size, bool *mask, 
					float &median, float &madfm);
template void findMedianStats<double>(double *array, int size, bool *mask, 
					 double &median, double &madfm);
//--------------------------------------------------------------------
  

template <class T> void findNormalStats(T *array, int size, 
					float &mean, float &stddev)
{
  /// @details
  /// Find the mean and rms or standard deviation of an array of
  /// numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param mean The mean value of the array, returned as a float.
  /// \param stddev The rms or standard deviation of the array,
  /// returned as a float.

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
  for(int i=1;i<size;i++) stddev += (array[i]-mean)*(array[i]-mean);
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

template <class T> void findNormalStats(T *array, int size, bool *mask, 
					float &mean, float &stddev)
{
  /// @details
  /// Find the mean and rms or standard deviation of a subset of an
  /// array of numbers. The subset is defined by an array of bool
  /// variables. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only look
  /// at values where mask=true.
  /// \param mean The mean value of the array, returned as a float.
  /// \param stddev The rms or standard deviation of the array,
  /// returned as a float.

  int goodSize=0;
  for(int i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findNormalStats: no good values!\n";
    return;
  }
  int start=0;
  while(!mask[start]){start++;}
  mean = array[start];
  for(int i=start+1;i<size;i++){
    if(mask[i]) mean += array[i];
  }
  mean /= float(goodSize);

  stddev = (array[start]-mean) * (array[start]-mean);
  for(int i=1;i<size;i++){
    if(mask[i]) stddev += (array[i]-mean)*(array[i]-mean);
  }
  stddev = sqrt(stddev/float(goodSize-1));

}
template void findNormalStats<int>(int *array, int size, bool *mask, 
				      float &mean, float &stddev);
template void findNormalStats<long>(long *array, int size, bool *mask, 
				       float &mean, float &stddev);
template void findNormalStats<float>(float *array, int size, bool *mask, 
					float &mean, float &stddev);
template void findNormalStats<double>(double *array, int size, bool *mask, 
					 float &mean, float &stddev);
//--------------------------------------------------------------------
//--------------------------------------------------------------------
  

template <class T> void findAllStats(T *array, int size, 
				     float &mean, float &stddev,
				     T &median, T &madfm)
{
  /// @details
  /// Find the mean,rms (or standard deviation), median AND madfm of an
  /// array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param mean The mean value of the array, returned as a float.
  /// \param stddev The rms or standard deviation of the array,
  /// returned as a float.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.

  if(size==0){
    std::cerr << "Error in findAllStats: zero sized array!\n";
    return;
  }

  T *newarray = new T[size];

  for(int i=0;i<size;i++) newarray[i] = array[i];

  mean = findMean(newarray,size);
  stddev = findStddev(newarray,size);
  median = findMedian(newarray,size,true);
  madfm = findMADFM(newarray,size,true);

  delete [] newarray;

}
template void findAllStats<int>(int *array, int size, 
				float &mean, float &stddev,
				int &median, int &madfm);
template void findAllStats<long>(long *array, int size, 
				 float &mean, float &stddev,
				 long &median, long &madfm);
template void findAllStats<float>(float *array, int size, 
				  float &mean, float &stddev,
				  float &median, float &madfm);
template void findAllStats<double>(double *array, int size, 
				   float &mean, float &stddev,
				   double &median, double &madfm);
//--------------------------------------------------------------------

template <class T> void findAllStats(T *array, int size, bool *mask, 
				     float &mean, float &stddev,
				     T &median, T &madfm)
{
  /// @details
  /// Find the mean,rms (or standard deviation), median AND madfm of a
  /// subset of an array of numbers. Type independent.The subset is
  /// defined by an array of bool variables. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only look
  /// at values where mask=true.
  /// \param mean The mean value of the array, returned as a float.
  /// \param stddev The rms or standard deviation of the array,
  /// returned as a float.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.

  int goodSize=0;
  for(int i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findAllStats: no good values!\n";
    return;
  }

  T *newarray = new T[goodSize];
  goodSize=0;
  for(int i=0;i<size;i++) if(mask[i]) newarray[goodSize++] = array[i];

  mean = findMean(newarray,goodSize);
  stddev = findStddev(newarray,goodSize);
  median = findMedian(newarray,goodSize,true);
  madfm = findMADFM(newarray,goodSize,true);

  delete [] newarray;

}
template void findAllStats<int>(int *array, int size, bool *mask,
				float &mean, float &stddev,
				int &median, int &madfm);
template void findAllStats<long>(long *array, int size, bool *mask,
				 float &mean, float &stddev,
				 long &median, long &madfm);
template void findAllStats<float>(float *array, int size, bool *mask,
				  float &mean, float &stddev,
				  float &median, float &madfm);
template void findAllStats<double>(double *array, int size, bool *mask,
				   float &mean, float &stddev,
				   double &median, double &madfm);
//--------------------------------------------------------------------
