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

template <class T> void findMinMax(const T *array, const size_t size, 
				   T &min, T &max)
{
  /// @details
  /// A function to find the minimum and maximum values of a set of numbers.
  /// \param array The array of data values. Type independent.
  /// \param size The length of the array
  /// \param min The returned value of the minimum value in the array.
  /// \param max The returned value of the maximum value in the array.
  min = max = array[0];
  for(size_t i=1;i<size;i++) {
    if(array[i]<min) min=array[i];
    if(array[i]>max) max=array[i];
  }
}
template void findMinMax<int>(const int *array, const size_t size, 
				 int &min, int &max);
template void findMinMax<long>(const long *array, const size_t size, 
				  long &min, long &max);
template void findMinMax<float>(const float *array, const size_t size, 
				   float &min, float &max);
template void findMinMax<double>(const double *array, const size_t size, 
				    double &min, double &max);
//--------------------------------------------------------------------

template <class T> void findAllStats(T *array, size_t size, 
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

  for(size_t i=0;i<size;i++) newarray[i] = array[i];

  mean = findMean(newarray,size);
  stddev = findStddev(newarray,size);
  median = findMedian(newarray,size,true);
  //  madfm = findMADFM(newarray,size,true);
  madfm = findMADFM(newarray,size,median,true);

  delete [] newarray;

}
template void findAllStats<int>(int *array, size_t size, 
				float &mean, float &stddev,
				int &median, int &madfm);
template void findAllStats<long>(long *array, size_t size, 
				 float &mean, float &stddev,
				 long &median, long &madfm);
template void findAllStats<float>(float *array, size_t size, 
				  float &mean, float &stddev,
				  float &median, float &madfm);
template void findAllStats<double>(double *array, size_t size, 
				   float &mean, float &stddev,
				   double &median, double &madfm);
//--------------------------------------------------------------------

template <class T> void findAllStats(T *array, size_t size, bool *mask, 
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
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findAllStats: no good values!\n";
    return;
  }

  T *newarray = new T[goodSize];
  goodSize=0;
  for(size_t i=0;i<size;i++) if(mask[i]) newarray[goodSize++] = array[i];

  mean = findMean(newarray,goodSize);
  stddev = findStddev(newarray,goodSize);
  median = findMedian(newarray,goodSize,true);
  //madfm = findMADFM(newarray,goodSize,true);
  madfm = findMADFM(newarray,goodSize,median,true);

  delete [] newarray;

}
template void findAllStats<int>(int *array, size_t size, bool *mask,
				float &mean, float &stddev,
				int &median, int &madfm);
template void findAllStats<long>(long *array, size_t size, bool *mask,
				 float &mean, float &stddev,
				 long &median, long &madfm);
template void findAllStats<float>(float *array, size_t size, bool *mask,
				  float &mean, float &stddev,
				  float &median, float &madfm);
template void findAllStats<double>(double *array, size_t size, bool *mask,
				   float &mean, float &stddev,
				   double &median, double &madfm);
//--------------------------------------------------------------------
