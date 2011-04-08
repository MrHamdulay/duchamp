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

template <class T> float findMean(T *array, size_t size)
{
  /// @details
  /// Find the mean of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \return The mean value of the array, returned as a float
  double mean = 0; 
  for(size_t i=0;i<size;i++) mean += double(array[i]);
  if(size>0)
    mean /= double(size);
  return float(mean);
}
template float findMean<int>(int *array, size_t size);
template float findMean<long>(long *array, size_t size);
template float findMean<float>(float *array, size_t size);
template float findMean<double>(double *array, size_t size);
//--------------------------------------------------------------------

template <class T> float findMeanDiff(T *first, T *second, size_t size)
{
  /// @details
  /// Find the mean of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \return The mean value of the array, returned as a float
  double mean = 0; 
  for(size_t i=0;i<size;i++) mean += double(first[i]-second[i]);
  if(size>0)
    mean /= double(size);
  return float(mean);
}
template float findMeanDiff<int>(int *first, int *second, size_t size);
template float findMeanDiff<long>(long *first, long *second, size_t size);
template float findMeanDiff<float>(float *first, float *second, size_t size);
template float findMeanDiff<double>(double *first, double *second, size_t size);
//--------------------------------------------------------------------

template <class T> float findMean(T *array, bool *mask, size_t size)
{
  /// @details
  /// Find the mean of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only use
  /// values where mask=true.
  /// \param size The length of the array.
  /// \return The mean value of the array, returned as a float
  double mean = 0.;
  int ct=0;
  for(size_t i=0;i<size;i++){
    if(mask[i]){
      mean += double(array[i]);
      ct++;
    }
  }
  if(ct>0) mean /= double(ct);
  return float(mean);
}
template float findMean<int>(int *array, bool *mask, size_t size);
template float findMean<long>(long *array, bool *mask, size_t size);
template float findMean<float>(float *array, bool *mask, size_t size);
template float findMean<double>(double *array, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> float findMeanDiff(T *first, T *second, bool *mask, size_t size)
{
  /// @details
  /// Find the mean of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only use
  /// values where mask=true.
  /// \param size The length of the array.
  /// \return The mean value of the array, returned as a float
  double mean = 0.;
  int ct=0;
  for(size_t i=0;i<size;i++){
    if(mask[i]){
      mean += double(first[i]-second[i]);
      ct++;
    }
  }
  if(ct>0) mean /= double(ct);
  return float(mean);
}
template float findMeanDiff<int>(int *first, int *second, bool *mask, size_t size);
template float findMeanDiff<long>(long *first, long *second, bool *mask, size_t size);
template float findMeanDiff<float>(float *first, float *second, bool *mask, size_t size);
template float findMeanDiff<double>(double *first, double *second, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> float findStddev(T *array, size_t size)
{
  /// @details Find the rms or standard deviation of an array of
  /// numbers. Type independent. Calculated by iterating only once,
  /// using \sum x and \sum x^2 (no call to findMean)
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \return The rms value of the array, returned as a float

  // double mean = double(findMean(array,size));
  // double stddev = (double(array[0])-mean) * (double(array[0])-mean);
  // for(size_t i=1;i<size;i++) stddev += (double(array[i])-mean)*(double(array[i])-mean);
  // double stddev = stddev/double(size-1));

  T sumx=0,sumxx=0;
  double stddev=0;
  for(size_t i=0;i<size;i++){
    sumx += array[i];
    sumxx += (array[i]*array[i]);
  }
  if(size>0)
    stddev = sqrt(double(sumxx)/double(size) - double(sumx*sumx)/double(size*size));
  return float(stddev);
}
template float findStddev<int>(int *array, size_t size);
template float findStddev<long>(long *array, size_t size);
template float findStddev<float>(float *array, size_t size);
template float findStddev<double>(double *array, size_t size);
//--------------------------------------------------------------------

template <class T> float findStddevDiff(T *first, T *second, size_t size)
{
  /// @details Find the rms or standard deviation of an array of
  /// numbers. Type independent. Calculated by iterating only once,
  /// using \sum x and \sum x^2 (no call to findMean)
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \return The rms value of the array, returned as a float

  T sumx=0,sumxx=0;
  double stddev=0;
  for(size_t i=0;i<size;i++){
    sumx += (first[i]-second[i]);
    sumxx += ((first[i]-second[i])*(first[i]-second[i]));
  }
  if(size>0)
    stddev = sqrt(double(sumxx)/double(size) - double(sumx*sumx)/double(size*size));
  return float(stddev);
}
template float findStddevDiff<int>(int *first, int *second, size_t size);
template float findStddevDiff<long>(long *first, long *second, size_t size);
template float findStddevDiff<float>(float *first, float *second, size_t size);
template float findStddevDiff<double>(double *first, double *second, size_t size);
//--------------------------------------------------------------------

template <class T> float findStddev(T *array, bool *mask, size_t size)
{
  /// @details Find the rms or standard deviation of an array of
  /// numbers. Type independent. Calculated by iterating only once,
  /// using \sum x and \sum x^2 (no call to findMean)
  /// \param array The array of numbers.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only use
  /// values where mask=true.
  /// \param size The length of the array.
  /// \return The rms value of the array, returned as a float

  T sumx=0,sumxx=0;
  double stddev=0;
  int ct=0;
  for(size_t i=0;i<size;i++){
    if(mask[i]){
      sumx += array[i];
      sumxx += (array[i]*array[i]);
      ct++;
    }
  }
  if(ct>0)
    stddev = sqrt(double(sumxx)/double(ct) - double(sumx*sumx)/double(ct*ct));
  return float(stddev);
}
template float findStddev<int>(int *array, bool *mask, size_t size);
template float findStddev<long>(long *array, bool *mask, size_t size);
template float findStddev<float>(float *array, bool *mask, size_t size);
template float findStddev<double>(double *array, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> float findStddevDiff(T *first, T *second, bool *mask, size_t size)
{
  /// @details Find the rms or standard deviation of an array of
  /// numbers. Type independent. Calculated by iterating only once,
  /// using \sum x and \sum x^2 (no call to findMean)
  /// \param array The array of numbers.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only use
  /// values where mask=true.
  /// \param size The length of the array.
  /// \return The rms value of the array, returned as a float

  T sumx=0,sumxx=0;
  double stddev=0;
  int ct=0;
  for(size_t i=0;i<size;i++){
    if(mask[i]){
      sumx += (first[i]-second[i]);
      sumxx += ((first[i]-second[i])*(first[i]-second[i]));
      ct++;
    }
  }
  if(ct>0)
    stddev = sqrt(double(sumxx)/double(ct) - double(sumx*sumx)/double(ct*ct));
  return float(stddev);
}
template float findStddevDiff<int>(int *first, int *second, bool *mask, size_t size);
template float findStddevDiff<long>(long *first, long *second, bool *mask, size_t size);
template float findStddevDiff<float>(float *first, float *second, bool *mask, size_t size);
template float findStddevDiff<double>(double *first, double *second, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> void findNormalStats(T *array, size_t size, 
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
  for(size_t i=1;i<size;i++){
    mean += array[i];
  }
  mean /= float(size);

  stddev = (array[0]-mean) * (array[0]-mean);
  for(size_t i=1;i<size;i++) stddev += (array[i]-mean)*(array[i]-mean);
  stddev = sqrt(stddev/float(size-1));

}
template void findNormalStats<int>(int *array, size_t size, 
				      float &mean, float &stddev);
template void findNormalStats<long>(long *array, size_t size, 
				       float &mean, float &stddev);
template void findNormalStats<float>(float *array, size_t size, 
					float &mean, float &stddev);
template void findNormalStats<double>(double *array, size_t size, 
					 float &mean, float &stddev);
//--------------------------------------------------------------------

template <class T> void findNormalStats(T *array, size_t size, bool *mask, 
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
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findNormalStats: no good values!\n";
    return;
  }
  int start=0;
  while(!mask[start]){start++;}
  mean = array[start];
  for(size_t i=start+1;i<size;i++){
    if(mask[i]) mean += array[i];
  }
  mean /= float(goodSize);

  stddev = (array[start]-mean) * (array[start]-mean);
  for(size_t i=1;i<size;i++){
    if(mask[i]) stddev += (array[i]-mean)*(array[i]-mean);
  }
  stddev = sqrt(stddev/float(goodSize-1));

}
template void findNormalStats<int>(int *array, size_t size, bool *mask, 
				      float &mean, float &stddev);
template void findNormalStats<long>(long *array, size_t size, bool *mask, 
				       float &mean, float &stddev);
template void findNormalStats<float>(float *array, size_t size, bool *mask, 
					float &mean, float &stddev);
template void findNormalStats<double>(double *array, size_t size, bool *mask, 
					 float &mean, float &stddev);
//--------------------------------------------------------------------  

template <class T> void findNormalStatsDiff(T *first, T *second, size_t size, 
					    float &mean, float &stddev)
{
  /// @details
  /// Find the mean and rms or standard deviation of  the difference between two arrays of
  /// numbers. The difference is defined as first - second. Type independent.
  /// 
  /// \param first The first array
  /// \param second The second array
  /// \param size The length of the array.
  /// \param mean The mean value of the array, returned as a float.
  /// \param stddev The rms or standard deviation of the array,
  /// returned as a float.

  if(size==0){
    std::cerr << "Error in findNormalStats: zero sized array!\n";
    return;
  }
  mean = first[0]-second[0];
  for(size_t i=1;i<size;i++){
    mean += (first[i]-second[i]);
  }
  mean /= float(size);

  stddev = (first[0]-second[0]-mean) * (first[0]-second[0]-mean);
  for(size_t i=1;i<size;i++) stddev += (first[i]-second[i]-mean)*(first[i]-second[i]-mean);
  stddev = sqrt(stddev/float(size-1));

}
template void findNormalStatsDiff<int>(int *first, int *second, size_t size, 
				       float &mean, float &stddev);
template void findNormalStatsDiff<long>(long *first, long *second, size_t size, 
					float &mean, float &stddev);
template void findNormalStatsDiff<float>(float *first, float *second, size_t size, 
					 float &mean, float &stddev);
template void findNormalStatsDiff<double>(double *first, double *second, size_t size, 
					  float &mean, float &stddev);
//--------------------------------------------------------------------

template <class T> void findNormalStatsDiff(T *first, T *second, size_t size, bool *mask, 
					    float &mean, float &stddev)
{
  /// @details Find the mean and rms or standard deviation of the
  /// difference between two arrays of numbers, where some elements
  /// are masked out. The mask is defined by an array of bool
  /// variables, and the difference is defined as first - second. Type
  /// independent.
  /// 
  /// \param first The first array
  /// \param second The second array
  /// \param size The length of the array.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only look
  /// at values where mask=true.
  /// \param mean The mean value of the array, returned as a float.
  /// \param stddev The rms or standard deviation of the array,
  /// returned as a float.

  int goodSize=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findNormalStats: no good values!\n";
    return;
  }
  int start=0;
  while(!mask[start]){start++;}
  mean = first[start]-second[start];
  for(size_t i=start+1;i<size;i++){
    if(mask[i]) mean += (first[i]-second[i]);
  }
  mean /= float(goodSize);

  stddev = (first[start]-second[start]-mean) * (first[start]-second[start]-mean);
  for(size_t i=1;i<size;i++){
    if(mask[i]) stddev += (first[i]-second[i]-mean)*(first[i]-second[i]-mean);
  }
  stddev = sqrt(stddev/float(goodSize-1));

}
template void findNormalStatsDiff<int>(int *first, int *second, size_t size, bool *mask, 
				       float &mean, float &stddev);
template void findNormalStatsDiff<long>(long *first, long *second, size_t size, bool *mask, 
					float &mean, float &stddev);
template void findNormalStatsDiff<float>(float *first, float *second, size_t size, bool *mask, 
					 float &mean, float &stddev);
template void findNormalStatsDiff<double>(double *first, double *second, size_t size, bool *mask, 
					  float &mean, float &stddev);
//--------------------------------------------------------------------
